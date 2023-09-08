#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include "define.h"
#include "time_test.h"
#include "CnfParser.h"
#include <iostream>
#include <cstring> // 有memcpy
#include <vector> //存储状态函数
#include <unordered_set>
#include <algorithm>

class DPLLSolver
{
public:
	DPLLSolver(int minThreads, int maxThreads)
		: min_threads(minThreads), max_threads(maxThreads)
	{
	}
	bool BasicSol(Doubly_Linked_List<CnfParser::clause> clauses, int bool_num)
	{
		const int timeoutSeconds = 2800;
		const auto startTime = std::chrono::steady_clock::now();

		LiteralStatus* setting = new LiteralStatus[bool_num + 1]{ LiteralStatus::Unassigned };  //记录变元的正负
		bool result_flag = Basic_Sol_Recursive(clauses, setting, bool_num);

		// 检查是否超时
		const auto endTime = std::chrono::steady_clock::now();
		const auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
		if (elapsedSeconds >= timeoutSeconds)
		{
			// 如果超时，设置标志变量为 true，然后继续执行
			cancel_flag = true;
			result_flag = false; // 将结果设置为 false，表示超时
		}
		return result_flag;
	}
	bool OptimizeSol(Doubly_Linked_List<CnfParser::clause> clauses, int bool_num)
	{
		const int timeoutSeconds = 2800;
		const auto startTime = std::chrono::steady_clock::now();

		LiteralStatus* setting = new LiteralStatus[bool_num + 1]{ LiteralStatus::Unassigned };  //记录变元的正负
		bool result_flag = Optimize_Sol_Recursive(clauses, setting, bool_num);

		// 检查是否超时
		const auto endTime = std::chrono::steady_clock::now();
		const auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
		if (elapsedSeconds >= timeoutSeconds)
		{
			// 如果超时，设置标志变量为 true，然后继续执行
			cancel_flag = true;
			result_flag = false; // 将结果设置为 false，表示超时
		}
		return result_flag;
	}
	std::vector<LiteralStatus> Get_Result()
	{
		return this->setting_result;
	}
	std::vector<size_t> Get_Variable_Change()
	{
		return this->Variable_Change;
	}
private:
	int min_threads, max_threads;
	typedef std::pair<int, int> VariableInfo; //记录变元出现次数
	typedef std::pair<int, int> PN; //区分正负
	std::vector<LiteralStatus> setting_result; //记录变元的正负
	std::vector<size_t> Variable_Change; //体现每一个分支的变元变化
	bool cancel_flag = false; // 标志变量，用于判断是否超时
	int shortestClauseLength = 1e9;

	void UnitClausePropagation(Doubly_Linked_List<CnfParser::clause>& clauses, LiteralStatus setting[], PN calculater[], const int bool_num)
	{
		//定义解析
		CnfParser parser;
		while (true)
		{
			std::vector<int> unitLiterals; // 用于存储所有单子句的文字
			bool HaveUnitClause = false; //标志是否找到单子句
			// 遍历子句集，找出所有单子句
			for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause) {
				if (parser.IsUnitClause(*it_clause)) {
					HaveUnitClause = true;
					unitLiterals.push_back((*it_clause).literals.front()); //一次性扫描所有单子句放入unitLiterals中
				}
			}

			if (!HaveUnitClause) return;

			// 处理所有单子句
			for (int unitLiteral : unitLiterals)
			{
				setting[std::abs(unitLiteral)] = (unitLiteral > 0 ? LiteralStatus::True : LiteralStatus::False);

				for (auto it_clause = clauses.begin(); it_clause != clauses.end();)
				{
					bool removeFlag = false;

					if (parser.IsLiteral(*it_clause, unitLiteral)) { //有L
						for (auto it_literal = (*it_clause).literals.begin(); it_literal != (*it_clause).literals.end(); ++it_literal)
						{
							*it_literal > 0 ? calculater[std::abs(*it_literal)].first-- : calculater[std::abs(*it_literal)].second--; //删除子句
						}
						removeFlag = true;
						it_clause = clauses.remove_node(it_clause); //数据结构删除结点
					}
					else if (parser.IsLiteral(*it_clause, -unitLiteral)) { //有非L
						(*it_clause).literals.remove_node(-unitLiteral); // 删除非L文字
					}

					if (!removeFlag) ++it_clause;
				}
				calculater[std::abs(unitLiteral)] = { 0, 0 }; //原单子句计数清零
			}

		}
	}
	//分裂策略：选择一个子句集中出现次数最多的且正负都存在的文字为变元
	int SelectLiteral(const PN calculater[], const int bool_num)
	{
		int max_occurence_literal = 0;
		int  select_literal = 0;
		for (size_t i = 1; i <= bool_num; ++i)
		{
			int occurence = calculater[i].first + calculater[i].second;
			if (max_occurence_literal < occurence && calculater[i].first != 0 && calculater[i].second != 0)
			{
				select_literal = i;
				max_occurence_literal = occurence;
			}
		}

		return select_literal;
	}
	//分裂策略：优先考虑最短子句中出现次数最多的变元
	int MOM(Doubly_Linked_List<CnfParser::clause>& clauses, int bool_num)
	{
		std::vector<VariableInfo> variableInfo(bool_num + 1, {0, 0});

		for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
		{
			if ((*it_clause).literals.size() != shortestClauseLength) continue;
			for (auto it_literal = (*it_clause).literals.begin(); it_literal != (*it_clause).literals.end(); ++it_literal)
			{
				*it_literal > 0 ? variableInfo[std::abs(*it_literal)].first++ : variableInfo[std::abs(*it_literal)].second++;
			}
		}
		int max_score = 0;
		int selectLiteral = 0;

		//选择出现次数最多且在最短子句中的变元
		for (int i = 1; i <= bool_num; ++i) {
			//得分策略是将
			int score = (variableInfo[i].first + 1) * (variableInfo[i].second + 1);//防止出现0
			if (score > max_score) {
				max_score = score;
				selectLiteral = (variableInfo[i].first> variableInfo[i].second) ? i : -i;
			}
		}

		return selectLiteral;

	}
	// 基础DPLL求解器
	bool Basic_Sol_Recursive(Doubly_Linked_List<CnfParser::clause>& clauses, LiteralStatus setting[], const int bool_num)
	{
		if (cancel_flag)
			return true;

		//统计每个变元的出现次数
		PN* calculater = new PN[bool_num + 1]{ {0, 0} };

		for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
		{
			for (auto it_literal = (*it_clause).literals.begin(); it_literal != (*it_clause).literals.end(); ++it_literal)
			{
				// 正负
				*it_literal > 0 ? calculater[std::abs(*it_literal)].first++ : calculater[std::abs(*it_literal)].second++;
			}
		}

		// 单子句传播
		UnitClausePropagation(clauses, setting, calculater, bool_num);

		// 所有子句被满足（子句集为空），返回true
		if (clauses.empty())
		{
			this->setting_result = std::vector<LiteralStatus>(setting, setting + bool_num + 1);
			return true;
		}

		// 含有空子句，返回false
		for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
		{
			if ((*it_clause).literals.empty())
				return false;
		}

		// 分裂策略
		int select_literal = SelectLiteral(calculater, bool_num);

		// 搜索副本
		Doubly_Linked_List<CnfParser::clause> current_clauses = clauses;

		// 构建单子句集
		//构建单子句，用于存入分类策略选出的数
		CnfParser::clause clause_left;
		clause_left.literals.push_back(select_literal);
		current_clauses.push_front(clause_left);

		if (Basic_Sol_Recursive(current_clauses, setting, bool_num))
			return true;

		// 构建单子句集
		//构建单子句，用于存入分类策略选出的数
		CnfParser::clause clause_right;
		clause_right.literals.push_back(-select_literal);
		clauses.push_front(clause_right);

		return Basic_Sol_Recursive(clauses, setting, bool_num);
	}
	bool Optimize_Sol_Recursive(Doubly_Linked_List<CnfParser::clause> clauses, LiteralStatus setting[], int bool_num)
	{
		if (cancel_flag) return true;
		//统计每个变元的出现次数
		PN* calculater = new PN[bool_num + 1]{ {0, 0} };

		for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
		{
			for (auto it_literal = (*it_clause).literals.begin(); it_literal != (*it_clause).literals.end(); ++it_literal)
			{
				//计算每个文字出现次数，正负区分
				*it_literal > 0 ? calculater[std::abs(*it_literal)].first++ : calculater[std::abs(*it_literal)].second++;
			}
		}

		//单子句传播
		UnitClausePropagation(clauses, setting, calculater, bool_num);

		//如果所有子句都满足（为空集）
		if (clauses.empty())
		{
			this->setting_result = std::vector<LiteralStatus>(setting, setting + bool_num + 1); //将setting数组中内容复制到setting_result里
			Variable_Change.push_back(0);
			return true;
		}

		//含有空子句
		for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
		{
			if ((*it_clause).literals.empty())
			{
				Variable_Change.push_back(clauses.size());  //记录每次递归子句的变化
				return false;
			}
			
			if ((*it_clause).literals.size() < shortestClauseLength && (*it_clause).literals.size() > 1)
			{
				shortestClauseLength = (*it_clause).literals.size(); //记录下当前剩余子句集中的最短子句
			}
		}

		// 分裂策略
		int select_literal = 0;
	    select_literal = MOM(clauses, bool_num);

		// 构建单子句集
		
		//构建单子句，用于存入分类策略选出的数
		CnfParser::clause clause_left;
		clause_left.literals.push_back(select_literal);
		//并进去
		clauses.push_front(clause_left);
		LiteralStatus* setting_left = new LiteralStatus[bool_num + 1]{ LiteralStatus::Unassigned };
		std::memcpy(setting_left, setting, (bool_num + 1) * sizeof(LiteralStatus)); //内存位置复制数据实现O（1）复杂度

		if (Optimize_Sol_Recursive(clauses, setting_left, bool_num))
			return true;

		clauses.pop_front(); //弹出

		// 构建单子句集
		
		//构建单子句，用于存入分类策略选出的数
		CnfParser::clause clause_right;
		clause_right.literals.push_back(-select_literal);
		clauses.push_front(clause_right);
		LiteralStatus* setting_right = new LiteralStatus[bool_num + 1]{ LiteralStatus::Unassigned };
		std::memcpy(setting_right, setting, (bool_num + 1) * sizeof(LiteralStatus)); //内存位置复制数据实现O（1）复杂度

		return Optimize_Sol_Recursive(clauses, setting_right, bool_num);
	}
};

#endif
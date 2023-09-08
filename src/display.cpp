#include "../include/CnfParser.h"
#include "../include/define.h"
#include "../include/time_test.h"
#include "../include/DPLLSolver.h"
#include "../include/Hanidoku.h"
#include "Hanidoku.cpp"
#include <iostream>
#include <functional>


using namespace std;

Doubly_Linked_List<CnfParser::clause> clauses;
DPLLSolver dpll_solver(1,1);
pair <bool, long long> result_pair;
long long run_time;
bool result;
std::vector<LiteralStatus> status;
std::vector<size_t> Variable_Change;

void SAT_Display(int op)
{
	extern int bool_num;
	string filename;
	CnfParser parser;
	int count, choice;
	int hole_num = 0;
	switch (op)
	{
	case 1:
		count = 0;
		do
		{
			cout << "请输入文件路径:";
			cin >> filename;
			count++;
		} while (!parser.ReadCNFFile(filename, clauses) && count < 5);

		if (count == 5) cout << "文件读取失败次数过多，请重新选择操作" << endl;

		break;
	case 2:
		if (clauses.empty())
		{
			cout << "请先读入cnf文件" << endl;
			break;
		}
		//绑定成员函数和对象实例
		result_pair = MeasureTime(std::bind(&DPLLSolver::BasicSol, &dpll_solver, std::placeholders::_1, std::placeholders::_2), clauses, bool_num);
		result = result_pair.first; //true还是false
		run_time = result_pair.second; //耗费的时间

		if (result) cout << "该cnf文件有解" << endl;
		else  cout << "该cnf文件无解" << endl;

		clauses.clear();

		break;
	case 3:
		if (clauses.empty())
		{
			cout << "请先读入cnf文件" << endl;
			break;
		}

		result_pair = MeasureTime(std::bind(&DPLLSolver::OptimizeSol, &dpll_solver, std::placeholders::_1, std::placeholders::_2), clauses, bool_num);
		result = result_pair.first; //true还是false
		run_time = result_pair.second; //耗费的时间

		if (result) cout << "该cnf文件有解" << endl;
		else  cout << "该cnf文件无解" << endl;

		clauses.clear();

		break;
	case 4:
		cout << "输入你要的.res文件路径吧~:";
		cin >> filename;
		status = dpll_solver.Get_Result();
		Variable_Change = dpll_solver.Get_Variable_Change();
		parser.Res(status, Variable_Change, filename, result, run_time);
		break;
	
	default:
		break;
	}
}

void Hanidoku_Display(int op)
{
	extern int bool_num;
	string filename;
	CnfParser parser;
	int count, choice;
	int total_hole_num = 0;
	
	switch (op)
	{
	case 1:
	{
		cout << "请输入你要挖洞的数量：";
		cin >> total_hole_num;

		Hanidoku hanidoku;
		hanidoku.generate_Hanidoku(total_hole_num);
		while (true)
		{
			cout << '\n';
			cout << "你是否要求解该数独？（1 for yes, 2 for no, 3 for correct anwser)";
			cin >> choice;
			if (choice == 3) hanidoku.Sudoku_Solving();
			else if (choice == 1) {
				hanidoku.user_playing(total_hole_num);
			}
			else if (choice == 2) {
				cout << "您已退出（按0退出Sudoku模块）" << endl;
				break;
			}
			
		}
		
		break;
	}
	
	
	default:
		break;
	}
}
void SAT_Menu()
{
	cout << "Option1: 读取cnf文件" << endl;
	cout << "Option2: 基础SAT求解" << endl;
	cout << "Option3: 优化SAT求解" << endl;
	cout << "Option4: 输出res文件" << endl;
	cout << "Option0: 结束" << endl;
	cout << "请输入你的选择: ";
}
void Hanidoku_Menu()
{
	cout << "Option1: 挖洞法生成蜂窝数独格局" << endl;
	cout << "Option0: 结束" << endl;
	cout << "请输入你的选择: ";
}
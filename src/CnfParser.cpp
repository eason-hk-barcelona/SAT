#include "../include/CnfParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

int bool_num, clause_num;

// 读取cnf文件
bool CnfParser::ReadCNFFile(const std::string filename, Doubly_Linked_List<clause> &clauses)
{
    std::ifstream inFile(filename);
    
    if (!inFile)
    {
        // 处理打开失败的文件
        std::cerr << "文件打开失败,请重新读取" << std::endl;
        return false;
    }

    // 读取cnf文件成功
    std::cout << "成功读取cnf文件" << std::endl;

    std::string line; //一行行读取

    while (std::getline(inFile, line))
    {
        if (line.empty() || line[0] == 'c')
            continue;

        std::istringstream iss(line);

        if (line[0] == 'p')
        {
            // 匿名变量忽略cnf字符串
            std::string _;
            iss >> _ >> _ >> bool_num >> clause_num;
            continue;
        }

        CnfParser::clause clause;
        int literal;

        while (iss >> literal && literal != 0)
        {
            clause.literals.push_back(literal);//将读入每一行存入子句集数据结构
        }

        clauses.push_back(clause);
    }

    inFile.close();

    return true;
}

// 判断是否为单子句（只有一个文字）
bool CnfParser::IsUnitClause(clause clause)
{
    return (clause.literals.size() == 1);
}

// 判断子句中是否含有某个文字
bool CnfParser::IsLiteral(clause clause, int literal)
{
    for (auto it_literal = clause.literals.begin(); it_literal != clause.literals.end(); ++it_literal)
    {
        if (*it_literal == literal)
            return true;
    }

    return false;
}

void CnfParser::Res(std::vector<LiteralStatus> status, std::vector<size_t> Variable_Change, std::string filename, bool flag, long long runtime)
{
    extern int bool_num;
    std::ofstream outFile(filename);

    if (!outFile)
    {
        // 处理打开失败的文件
        std::cerr << "文件打开失败,请重新读取" << std::endl;
        return;
    }

    outFile.clear();

    if (!flag)
    {
        outFile << "s 0" << std::endl;
        outFile << "v" << std::endl;
        outFile << "t " << runtime << std::endl;

        outFile.close();
        return;
    }

    outFile << "s 1" << std::endl;
    outFile << "v";

    for (int i = 1; i <= bool_num; ++i)
    {
        if (status[i] == LiteralStatus::True)
        {
            outFile << " " << i;
        }
        else if (status[i] == LiteralStatus::False)
        {
            outFile << " " << -i;
        }
        else if (status[i] == LiteralStatus::Unassigned)
        {
            outFile << " " << i << "(Unassigned)";
        }
    }
    
    outFile << std::endl;

    outFile << "t " << runtime << std::endl;

    for (const int& size : Variable_Change) {
        std::cout << size << " ";
    }
    outFile.close();
}
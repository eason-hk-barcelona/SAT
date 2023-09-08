#include "define.h"
#include "CnfParser.h"
#include <fstream>
#include <sstream>
int bool_num, clause_num;
int main()
{
    DeLinkList<CnfParser::clause> clauses, clauses_test;
    string filename;
    CnfParser parser;
    
    extern int bool_num;

    cout<<"输入："<<endl;
    cin>> filename;
    parser.ReadCNFFile(filename, clauses);

    
    for(auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
    {
        CnfParser::clause clause_test;
        for(auto it_literal = (*it_clause).literals.begin(); it_literal!=(*it_clause).literals.end(); ++it_literal)
        {
            int select = *it_literal;
            clause_test.literals.push_back(select);
        }
        clauses_test.push_back(clause_test);
    }
    cout<<"ok"<<endl;
    system("pause");
    return 0;
}

// 读取cnf文件
bool CnfParser::ReadCNFFile(const std::string filename, DeLinkList<clause> &clauses)
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

    std::string line;
    CnfParser::clause clause;
    int literal;

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

        

        while (iss >> literal && literal != 0)
        {
            clause.literals.push_back(literal);
        }

        clauses.push_back(clause);
    }

    inFile.close();

    return true;
}

#ifndef CNFPARSER_H
#define CNFPARSER_H

#include "define.h"
#include <iostream>
#include <vector>

enum class LiteralStatus
{
    Unassigned,
    True,
    False,
};

class CnfParser
{
public:
     // 定义子句
    struct clause
    {
        // 子句中的文字
        Doubly_Linked_List<int> literals;
    };

    // 读取cnf文件
    bool ReadCNFFile(const string filename, Doubly_Linked_List<clause> &clauses);

    // 判断是否为单子句（只有一个文字）
    bool IsUnitClause(clause clause);

    // 判断子句中是否含有某个文字
    bool IsLiteral(clause clause, int literal);
    
    //输出结果
    void Res(std::vector<LiteralStatus> status, std::vector<size_t> Variable_Change, std::string filename, bool flag, long long runtime);
};

#endif
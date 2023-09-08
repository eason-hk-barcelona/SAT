#ifndef HANIDOKU_H
#define HANIDOKU_H
//
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CnfParser.h"
#include "DPLLSolver.h"
#include "define.h"

using namespace std;

class Hanidoku
{
public:
    Hanidoku()
    {
        Rules_Initialization();

        CnfParser parser;

        parser.ReadCNFFile(this->filename, this->clauses);
    }

    //用户交互求解
    void user_playing(int total_hole_num)
    {
        int index = -1, anwser = -1, iteration = 0;

        while (iteration != total_hole_num)
        {
            cout << "\n请输入你要填入的数所在的索引（从左至右，从上到下顺序，若选择-1则直接退出求解）：";
            cin >> index;
            cout << "请输入你的答案：";
            cin >> anwser;
            for (int i = 1; i < 62; ++i)
            {
                if (Holes[i].first == index && Holes[i].second == anwser)
                {
                    cout << "该数正确！" << endl;
                    iteration++;
                    break;
                }
                else if (Holes[i].first == index && Holes[i].second != anwser)
                {
                    cout << "该数不正确哦！再仔细想一想吧！" << endl;
                    break;
                }
                else if (index == -1)break;
            }
            if (index == -1) break;
        }
        if (index != -1) cout << "恭喜你，完成了所有数独的填空！" << endl;
        else cout << "你已退出求解" << endl;

    }

    // 生成数独
    void generate_Hanidoku(int total_hole_num)
    {
        // 随机化种子
        srand((int)time(0));

        // 获取游戏终盘
        int game_index = rand() % 20;
        string game_data = Valid_Game_Distribution[game_index];
        get_correct_result = Valid_Game_Distribution[game_index];
        // 挖洞生成唯一解的蜂窝数独
        for (int current_hole_num = 1; current_hole_num <= total_hole_num; ++current_hole_num)
        {
            // 只要当前位置是0，换一个继续挖
            int index = -1;
            do
            {
                index = rand() % 61;
            } while (game_data[index] == '0');

            // 拷贝游戏终盘副本
            string current_game_data = game_data;

            // 挖去该洞以后解的数量
            int sum_answer = 0;

            for (int k = 1; k <= 9; ++k)
            {
                current_game_data[index] = '0' + k;
                if (dpllsolver(current_game_data))//只要能解出来
                    ++sum_answer;
                if (sum_answer > 1)
                    break;
            }

            if (sum_answer == 1)
            {
                game_data[index] = '0';
                cout << "挖好第 " << current_hole_num << " 个洞" << endl;
            }
            else
            {
                cout << "回溯" << endl;
                current_hole_num--;
            }
        }

        // 记录当前数独终盘并在终端上打印
        int game[62] = { 0 };

        for (int index = 0; index < 61; ++index)
        {
            game[index + 1] = game_data[index] - '0';

            if (game_data[index] == '0')
                continue;

            CnfParser::clause clause_temp;
            clause_temp.literals.push_back(game_data[index] - '0' + index * 9); //一个位子占九个变元数量
            this->clauses.push_front(clause_temp);
        }

        Cnf_Creation(this->filename, this->clauses);

        windowshowcase(game);
    }

    

    // 求解数独
    void Sudoku_Solving()
    {
        extern DPLLSolver dpll_solver;

        dpll_solver.OptimizeSol(this->clauses, 549);

        vector<LiteralStatus> temp = dpll_solver.Get_Result(); //549个变元的赋值

        int sol_result[62] = { 0 };

        for (int index = 0; index < 61; ++index)
        {
            for (int k = 1; k <= 9; ++k)
            {
                if (temp[index * 9 + k] == LiteralStatus::True)// 赋值为真
                    sol_result[index + 1] = k;
            }
        }

        windowshowcase(sol_result);
    }
private:
    Doubly_Linked_List<CnfParser::clause> clauses;
    string filename;
    string get_correct_result;
    typedef pair<int, char> holes;
    holes* Holes = new holes[63]{ {-1, -1} }; //记录挖空的答案
    int row_start[9] = { 0, 5, 11, 18, 26, 35, 43, 50, 56 }; //每行开头的格子索引
    const vector<int> Rows = { 5, 6, 7, 8, 9, 8, 7, 6, 5 }; // 每行的格子数
    string Valid_Game_Distribution[20] = { "4563263847554721632638975437915428625789634463217554876363452",
                                        "4536232457667984532367984556813792445281673745283631456263745",
                                        "7856963748554123768635124795276413874128365634125787354656789",
                                        "5364246723575984636237985434598271678361452451726367483553624",
                                        "3647567853474521634536928759218374687321465684725346859775684",
                                        "5436263724575986343247985646598271378431652651723437684554326",
                                        "3574676458364132754352186758217639476839245375248669485745768",
                                        "5634248765335124762769853464273918535892476436875251426376534",
                                        "3675461543275621435428937643192768524379856376854268547356234",
                                        "3675454231661572434361852772489163551237486367285445876363542",
                                        "4632558476337152466529783424683951737492865531672468745345236",
                                        "5426342531626317546317842531659284754821763724863545738663574",
                                        "7436563584747126535829347636758129454312678672348549685778546",
                                        "2435653641262785433589276446978123574396582524317667845335764",
                                        "5236464573247986533542987626831794537986452657983442357654623",
                                        "6845754679887935644582137676398214537461528582437663725445637",
                                        "5426376354263721544251873637842961556398742487695375483663524",
                                        "5637461542374631524389726535928174627594863486732563857456432",
                                        "5632438465747251366387241524519876357861324637241548653735246",
                                        "2436557624363451274581273636278951427396845587493643865765423" };

    // 求解数独
    bool dpllsolver(string game_data)
    {
        Doubly_Linked_List<CnfParser::clause> clauses_temp = this->clauses;

        extern DPLLSolver dpll_solver;

        for (int index = 0; index < 61; ++index)
        {
            if (game_data[index] == '0')
                continue;

            CnfParser::clause clause_temp;
            clause_temp.literals.push_back(game_data[index] - '0' + index * 9);
            clauses_temp.push_front(clause_temp);
        }

        return dpll_solver.OptimizeSol(clauses_temp, 549);
    }
    // 自然顺序编码：ijn → row_start[ - 1] * 9+(j-1)*9+n
    int Natural_Sequence_Encoding(int literal)
    {
        int variable = 0;
        int x = literal % 10;

        variable += x;

        literal = literal / 10;
        x = literal % 10;

        variable += (x - 1) * 9;

        x = literal / 10;

        variable += row_start[x - 1] * 9;

        return variable;
    }
    // 生成行,对角线,选填，必填约束的子句
    void clauses_for_constraints(string filename, vector<int> positions, int rows_length)
    {
        ofstream outFile(filename, ios::app); // 打开文件并且追加内容

        if (!outFile)
        {
            // 处理打开失败的文件
            cerr << "文件打开失败,请重新读取" << endl;
            return;
        }
        //必填约束
        switch (rows_length)
        {
        case 5:
            // 必填数字5
            for (int i = 5; i <= 5; ++i)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i; //语义编码的位置
                    literal = Natural_Sequence_Encoding(literal); //自然编码
                    outFile << literal << " ";
                }
                outFile << "0" << endl;
            }

            break;

        case 6:
            // 必填数字456
            for (int i = 4; i <= 6; ++i)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }
                outFile << "0" << endl;
            }

            break;

        case 7:
            // 必填数字34567
            for (int i = 3; i <= 7; ++i)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }
                outFile << "0" << endl;
            }

            break;

        case 8:
            // 必填数字2345678
            for (int i = 2; i <= 8; ++i)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }
                outFile << "0" << endl;
            }

            break;

        case 9:
            // 必填数字1-9
            for (int i = 1; i <= 9; ++i)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }
                outFile << "0" << endl;
            }

            break;

        default:
            break;
        }

        // 选填数字
        for (int i = rows_length + 1; i <= 9; ++i)
        {
            for (int j = i - rows_length; j <= 9 - rows_length; ++j)
            {
                for (const auto position : positions)
                {
                    int literal = position * 10 + i;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }

                for (const auto position : positions)
                {
                    int literal = position * 10 + j;
                    literal = Natural_Sequence_Encoding(literal);
                    outFile << literal << " ";
                }

                outFile << "0" << endl;
            }
        }

        // 任意两个格子不可以填写相同的数字
        for (int i = 1; i <= 9; ++i)
        {
            for (int index1 = 0; index1 < rows_length; ++index1)
            {
                for (int index2 = index1 + 1; index2 < rows_length; ++index2)
                {
                    int literal1 = positions[index1] * 10 + i;
                    int literal2 = positions[index2] * 10 + i;
                    literal1 = Natural_Sequence_Encoding(literal1);
                    literal2 = Natural_Sequence_Encoding(literal2);
                    outFile << -literal1 << " " << -literal2 << " 0" << endl;
                }
            }
        }

        outFile.close();
    }

    // 数独规则
    void Rules_Initialization()
    {
        cout << "请输入生成的cnf文件路径: ";
        cin >> this->filename;

        ofstream outFile(filename);

        if (!outFile)
        {
            // 处理打开失败的文件
            cerr << "文件打开失败,请重新读取" << endl;
            return;
        }

        outFile << "p cnf " << 549 << " " << 8848 << endl;//任意初始化

        // 格约束
        for (int i = 0; i < 61; ++i)
        {
            //当前格可以填写1-9
            for (int k = 1; k <= 9; ++k)
            {
                outFile << i * 9 + k << " ";
            }
            outFile << "0" << endl;

            //每一格只能填入一个数字
            for (int k1 = 1; k1 <= 9; ++k1)
            {
                for (int k2 = k1 + 1; k2 <= 9; ++k2)
                {
                    outFile << -(i * 9 + k1) << " " << -(i * 9 + k2) << " 0" << endl;
                }
            }
        }

        outFile.close();

        // 5 格的规则
        clauses_for_constraints(filename, vector<int>({ 11, 12, 13, 14, 15 }), 5);
        clauses_for_constraints(filename, vector<int>({ 11, 21, 31, 41, 51 }), 5);
        clauses_for_constraints(filename, vector<int>({ 51, 61, 71, 81, 91 }), 5);
        clauses_for_constraints(filename, vector<int>({ 91, 92, 93, 94, 95 }), 5);
        clauses_for_constraints(filename, vector<int>({ 59, 68, 77, 86, 95 }), 5);
        clauses_for_constraints(filename, vector<int>({ 15, 26, 37, 48, 59 }), 5);

        // 6 格的规则
        clauses_for_constraints(filename, vector<int>({ 21, 22, 23, 24, 25, 26 }), 6);
        clauses_for_constraints(filename, vector<int>({ 12, 22, 32, 42, 52, 61 }), 6);
        clauses_for_constraints(filename, vector<int>({ 41, 52, 62, 72, 82, 92 }), 6);
        clauses_for_constraints(filename, vector<int>({ 81, 82, 83, 84, 85, 86 }), 6);
        clauses_for_constraints(filename, vector<int>({ 48, 58, 67, 76, 85, 94 }), 6);
        clauses_for_constraints(filename, vector<int>({ 14, 25, 36, 47, 58, 68 }), 6);

        // 7 格的规则
        clauses_for_constraints(filename, vector<int>({ 31, 32, 33, 34, 35, 36, 37 }), 7);
        clauses_for_constraints(filename, vector<int>({ 13, 23, 33, 43, 53, 62, 71 }), 7);
        clauses_for_constraints(filename, vector<int>({ 31, 42, 53, 63, 73, 83, 93 }), 7);
        clauses_for_constraints(filename, vector<int>({ 71, 72, 73, 74, 75, 76, 77 }), 7);
        clauses_for_constraints(filename, vector<int>({ 37, 47, 57, 66, 75, 84, 93 }), 7);
        clauses_for_constraints(filename, vector<int>({ 13, 24, 35, 46, 57, 67, 77 }), 7);

        // 8 格的规则
        clauses_for_constraints(filename, vector<int>({ 41, 42, 43, 44, 45, 46, 47, 48 }), 8);
        clauses_for_constraints(filename, vector<int>({ 14, 24, 34, 44, 54, 63, 72, 81 }), 8);
        clauses_for_constraints(filename, vector<int>({ 21, 32, 43, 54, 64, 74, 84, 94 }), 8);
        clauses_for_constraints(filename, vector<int>({ 61, 62, 63, 64, 65, 66, 67, 68 }), 8);
        clauses_for_constraints(filename, vector<int>({ 26, 36, 46, 56, 65, 74, 83, 92 }), 8);
        clauses_for_constraints(filename, vector<int>({ 12, 23, 34, 45, 56, 66, 76, 86 }), 8);

        // 9 格的规则
        clauses_for_constraints(filename, vector<int>({ 51, 52, 53, 54, 55, 56, 57, 58, 59 }), 9);
        clauses_for_constraints(filename, vector<int>({ 15, 25, 35, 45, 55, 64, 73, 82, 91 }), 9);
        clauses_for_constraints(filename, vector<int>({ 11, 22, 33, 44, 55, 65, 75, 85, 95 }), 9);
    }

    

    // 输出cnf文件
    void Cnf_Creation(string filename, Doubly_Linked_List<CnfParser::clause> clauses)
    {
        ofstream outFile(filename);

        if (!outFile)
        {
            // 处理打开失败的文件
            cerr << "文件打开失败,请重新读取" << endl;
            return;
        }

        outFile << "p cnf " << 549 << " " << clauses.size() << endl; //变元数量，子句数量

        for (auto it_clause = clauses.begin(); it_clause != clauses.end(); ++it_clause)
        {
            for (auto it_literal = (*it_clause).literals.begin(); it_literal != (*it_clause).literals.end(); ++it_literal)
            {
                outFile << *it_literal << " ";
            }
            outFile << "0" << endl;
        }

        outFile.close();
    }

    // 打印数独
    void windowshowcase(int sol_result[])
    {
        int iteration = 1;
        char solution[63];
        for (int i = 1; i < 62; ++i)
        {
            if (sol_result[i] == 0)
            {
                solution[i] = ' ';
                Holes[iteration].first = i;
                Holes[iteration].second = get_correct_result[i-1]-'0';
                iteration++;
            }
            else solution[i] = '0' + sol_result[i];
        }
        solution[62] = '\0';

        printf("         / \\ / \\ / \\ / \\ / \\  \n");
        printf("        | %c | %c | %c | %c | %c |  \n", solution[1], solution[2], solution[3], solution[4], solution[5]);
        printf("       / \\ / \\ / \\ / \\ / \\ / \\  \n");
        printf("      | %c | %c | %c | %c | %c | %c |  \n", solution[6], solution[7], solution[8], solution[9], solution[10], solution[11]);
        printf("     / \\ / \\ / \\ / \\ / \\ / \\ / \\  \n");
        printf("    | %c | %c | %c | %c | %c | %c | %c |  \n", solution[12], solution[13], solution[14], solution[15], solution[16], solution[17], solution[18]);
        printf("   / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\  \n");
        printf("  | %c | %c | %c | %c | %c | %c | %c | %c |  \n", solution[19], solution[20], solution[21], solution[22], solution[23], solution[24], solution[25], solution[26]);
        printf(" / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\  \n");
        printf("| %c | %c | %c | %c | %c | %c | %c | %c | %c |  \n", solution[27], solution[28], solution[29], solution[30], solution[31], solution[32], solution[33], solution[34], solution[35]);
        printf(" \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ /  \n");
        printf("  | %c | %c | %c | %c | %c | %c | %c | %c |  \n", solution[36], solution[37], solution[38], solution[39], solution[40], solution[41], solution[42], solution[43]);
        printf("   \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ /  \n");
        printf("    | %c | %c | %c | %c | %c | %c | %c |  \n", solution[44], solution[45], solution[46], solution[47], solution[48], solution[49], solution[50]);
        printf("     \\ / \\ / \\ / \\ / \\ / \\ / \\ /  \n");
        printf("      | %c | %c | %c | %c | %c | %c |  \n", solution[51], solution[52], solution[53], solution[54], solution[55], solution[56]);
        printf("       \\ / \\ / \\ / \\ / \\ / \\ /  \n");
        printf("        | %c | %c | %c | %c | %c |  \n", solution[57], solution[58], solution[59], solution[60], solution[61]);
        printf("         \\ / \\ / \\ / \\ / \\ /  \n");
    }
};

#endif

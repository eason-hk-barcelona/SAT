#include "../include/display.h"
#include "../include/cue.h"
#include <iostream>
#include <stdlib.h>
#pragma pack (16)

int main()
{
    int op = 0;
    cue();
    std::cin >> op;
    while (true)
    {
        if (op == 1) {
            Hanidoku_Menu();
            do
            {
                std::cin >> op;
                Hanidoku_Display(op);
            } while (op);
        }
        else if (op == 2) {
            SAT_Menu();
            do
            {
                std::cin >> op;
                SAT_Display(op);
            } while (op);
        }
        else if (op == 0) {
            std::cout << "感谢使用本系统呦！" << std::endl;
            break;
        }
        std::cout << "\n";
        cue();
        std::cin >> op;
    }
    system("pause");
    return 0;
}
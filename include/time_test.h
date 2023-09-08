#ifndef TIME_TEST_H
#define TIME_TEST_H

#include<iostream>
#include <chrono>
#include <functional>
// 时间测试函数
template <typename Func, typename... Args>
auto MeasureTime(Func func, Args... args)
{
    //记录开始时间
    auto start_time = std::chrono::high_resolution_clock::now();

    // 调用传入的函数并且传递参数
    auto result = func(args...);

    // 记录结束时间点
    auto end_time = std::chrono::high_resolution_clock::now();

    // 计算执行时间的持续时间，以毫秒为单位
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "运行时间为： " << duration.count() << "ms" << std::endl;

    return std::make_pair(result, duration.count());
}

#endif
#include <iostream>
#include <vector>
#include <chrono>

#include "ThreadPool.h"

int main()
{
    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 16; ++i)
    {
        results.emplace_back(
            pool.enqueue([i]
                         {
                std::cout << "Task " << i << " started" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "Task " << i << " finished" << std::endl;
                return i * i; }));
    }

    for (auto &&result : results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;
}

#include <algorithm>
#include <cstddef>
#include <future>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
#include <windows.h>

namespace parallel {

    template <typename Iterator, typename Func>
    void for_each_impl(Iterator first, Iterator last, Func& func, std::size_t min_block) {
        const auto length = std::distance(first, last);

        if (length <= 0) {
            return;
        }

        if (static_cast<std::size_t>(length) <= min_block) {
            std::for_each(first, last, func);
            return;
        }

        Iterator mid = first;
        std::advance(mid, length / 2);

        auto left = std::async(std::launch::async,
            [first, mid, &func, min_block]() {
                for_each_impl(first, mid, func, min_block);
            });

        auto right = std::async(std::launch::async,
            [mid, last, &func, min_block]() {
                for_each_impl(mid, last, func, min_block);
            });

        left.get();
        right.get();
    }

    template <typename Iterator, typename Func>
    void for_each(Iterator first, Iterator last, Func& func, std::size_t min_block = 1000) {
        for_each_impl(first, last, func, min_block);
    }

} 

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    std::vector<int> data(10000);
    std::iota(data.begin(), data.end(), 1); 

    auto square = [](int& x) {
        x *= x;
        };

    parallel::for_each(data.begin(), data.end(), square);

    std::cout << "data[0]     = " << data[0] << '\n';
    std::cout << "data[1]     = " << data[1] << '\n';
    std::cout << "data[9999]  = " << data[9999] << '\n';

    return 0;
}

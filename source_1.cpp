#include <algorithm>
#include <cstddef>
#include <future>
#include <iostream>
#include <utility>
#include <vector>
#include <windows.h>

std::future<void> selectionSortAsync(std::vector<int>& data, std::size_t begin) {

    if (begin + 1 >= data.size()) {
        std::promise<void> done;
        done.set_value();
        return done.get_future();
    }

    std::promise<std::size_t> minPromise;
    std::future<std::size_t> minFuture = minPromise.get_future();

    std::future<void> searchFuture = std::async(
        std::launch::async,
        [&data, begin, p = std::move(minPromise)]() mutable {
            std::size_t minIndex = begin;

            for (std::size_t j = begin + 1; j < data.size(); ++j) {
                if (data[j] < data[minIndex]) {
                    minIndex = j;
                }
            }

            p.set_value(minIndex);
        });

    const std::size_t minIndex = minFuture.get();

    searchFuture.get();

    if (minIndex != begin) {
        std::swap(data[begin], data[minIndex]);
    }

    return std::async(std::launch::async, [&data, begin]() {
        selectionSortAsync(data, begin + 1).get();
        });
}

void printVector(const std::vector<int>& v) {
    for (int x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

int main() {

    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    std::vector<int> data = { 9, 4, 7, 1, 8, 2, 6, 3, 5, 0 };

    std::cout << "До сортировки:    ";
    printVector(data);

    selectionSortAsync(data, 0).get();

    std::cout << "После сортировки: ";
    printVector(data);

    return 0;
}

// Copyright 2024 Severin Denisenko

#include <folly/executors/CPUThreadPoolExecutor.h>

#include <boost/format.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

int main()
{
    uint32_t threads = 5;
    folly::CPUThreadPoolExecutor executor { threads };

    std::mutex mtx;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10);

    uint32_t works = 5;
    for (uint32_t work = 0; work < works; ++work) {
        executor.add([work, &mtx, &rng, &dist]() {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms * dist(rng));

            {
                std::lock_guard lock { mtx };
                std::cout << boost::format("Work %1% done!\n") % work;
            }
        });
    }

    executor.join();
}

// Copyright 2024 Severin Denisenko

#include <folly/FBString.h>
#include <folly/Function.h>
#include <folly/Try.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>

#include <boost/format.hpp>

#include <iostream>
#include <stdexcept>

class Worker {
public:
    template <typename Res>
    folly::Future<Res> doWork(folly::Function<Res(void)> work)
    {
        folly::Promise<Res> promise;
        folly::Future<Res> future = promise.getFuture();
        executor_.add(
            [promise = std::move(promise), work = std::move(work)]() mutable { std::move(promise).setValue(work()); });
        return std::move(future);
    }

private:
    // ThreadedExecutor spawns new tread for each task
    folly::ThreadedExecutor executor_;
};

int main()
{
    Worker worker;

    // folly::Unit is like void
    folly::Future<folly::Unit> future = worker
                                            .doWork<folly::fbstring>([]() -> folly::fbstring {
                                                folly::fbstring input;
                                                std::cin >> input;
                                                return input;
                                            })
                                            .thenValue([](folly::fbstring&& input) {
                                                if (input == "Error") {
                                                    // throwed exception will end up in folly::Try<>::exeption()
                                                    throw std::runtime_error("Error");
                                                } else {
                                                    return input;
                                                }
                                            })
                                            .then([](folly::Try<folly::fbstring>&& result) {
                                                if (result.hasValue()) {
                                                    std::cout << result.value() << std::endl;
                                                } else if (result.hasException()) {
                                                    std::cout << "Error!" << std::endl;
                                                }
                                            });

    future.wait();

    return 0;
}

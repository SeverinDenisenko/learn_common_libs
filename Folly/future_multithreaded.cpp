// Copyright 2024 Severin Denisenko

#include <folly/Function.h>
#include <folly/Try.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>

#include <boost/format.hpp>

#include <iostream>

class Worker {
public:
    template <typename Res>
    folly::Future<Res> doWork(folly::Function<Res(folly::Unit)> work)
    {
        folly::Promise<Res> promise;
        folly::Future<Res> future = promise.getFuture();
        executor_.add([promise = std::move(promise), work = std::move(work)]() mutable {
            std::move(promise).setValue(work(folly::Unit {}));
        });
        return std::move(future);
    }

    folly::ThreadedExecutor& getExecutor()
    {
        return executor_;
    }

private:
    folly::ThreadedExecutor executor_;
};

int main()
{
    Worker worker_1;
    Worker worker_2;

    auto first = worker_1
                     .doWork<folly::Unit>([](folly::Unit&&) -> folly::Unit {
                         std::cout << "From thread 1 first" << std::endl;
                         return {};
                     })
                     .via(&worker_2.getExecutor())
                     .thenValue([](folly::Unit&&) -> folly::Unit {
                         std::cout << "From thread 2 first" << std::endl;
                         return {};
                     });

    auto second = worker_2
                      .doWork<folly::Unit>([](folly::Unit&&) -> folly::Unit {
                          std::cout << "From thread 2 second" << std::endl;
                          return {};
                      })
                      .via(&worker_1.getExecutor())
                      .thenValue([](folly::Unit&&) -> folly::Unit {
                          std::cout << "From thread 1 second" << std::endl;
                          return {};
                      });

    // folly::collectAll combines future into one
    folly::collectAll(first, second).wait();

    return 0;
}

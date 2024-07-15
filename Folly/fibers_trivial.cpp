// Copyright 2024 Severin Denisenko

#include <iostream>

#include <folly/FBString.h>
#include <folly/fibers/AddTasks.h>
#include <folly/fibers/FiberManager.h>
#include <folly/fibers/FiberManagerMap.h>
#include <folly/fibers/GenericBaton.h>
#include <folly/fibers/Promise.h>
#include <folly/io/async/EventBase.h>

class App {
public:
    App()
        : evb_ {}
        , manager_ { folly::fibers::getFiberManager(evb_) }
    {
    }

    void Run()
    {
        folly::Function<void(void)> task = [this]() {
            manager_.addTask([this] {
                while (true) {
                    std::cin >> str_;
                    baton_writer_.post();
                    baton_reader_.wait();
                }
            });

            manager_.addTask([this] {
                while (true) {
                    baton_writer_.wait();
                    std::cout << str_ << std::endl;
                    baton_reader_.post();
                }
            });
        };

        manager_.addTask(task);

        evb_.loop();
    }

private:
    folly::EventBase evb_;
    folly::fibers::FiberManager& manager_;

    folly::fibers::Baton baton_reader_;
    folly::fibers::Baton baton_writer_;
    folly::fbstring str_;
};

int main()
{
    App app;
    app.Run();

    return 0;
}

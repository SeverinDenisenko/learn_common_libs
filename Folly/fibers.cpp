// Copyright 2024 Severin Denisenko

#include <iostream>

#include <folly/fibers/AddTasks.h>
#include <folly/fibers/FiberManager.h>
#include <folly/fibers/FiberManagerMap.h>
#include <folly/fibers/GenericBaton.h>
#include <folly/io/async/EventBase.h>

int main()
{
    folly::EventBase evb;

    auto& fiberManager = folly::fibers::getFiberManager(evb);
    folly::fibers::Baton baton;
    fiberManager.addTask([&]{
        std::cout << "Task: start" << std::endl;
        baton.wait();
        std::cout << "Task: after baton.wait()" << std::endl;
    });

    baton.post();
    std::cout << "Baton posted" << std::endl;

    evb.loop();

    return 0;
}

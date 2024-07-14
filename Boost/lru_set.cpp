// Copyright 2024 Severin Denisenko

#include <iostream>

#include <fmt/format.h>

#include "lru_set.hpp"

template <typename T>
void print(lru_set<T>& set)
{
    for (const auto& val : set) {
        std::cout << fmt::format("{} ", val.get_key());
    }
    std::cout << std::endl;
}

int main()
{
    lru_set<int> set { 3 };

    set.put(1);
    set.put(2);
    set.put(3);

    print(set);

    set.put(4);

    print(set);

    set.put(4);
    set.put(4);
    set.put(4);

    print(set);

    return 0;
}

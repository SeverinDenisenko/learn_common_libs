// Copyright 2024 Severin Denisenko

#include <iostream>

#include <fmt/format.h>

#include "lru_map.hpp"

template <typename T, typename U>
void print(lru_map<T, U>& map)
{
    for (const auto& val : map) {
        std::cout << fmt::format("({},  {}) ", val.get_key(), val.get_value());
    }
    std::cout << std::endl;
}

int main()
{
    lru_map<int, int> map { 3 };

    map.put(1, 1);
    map.put(2, 2);
    map.put(3, 3);

    print(map);

    map.put(4, 4);

    print(map);

    map.put(4, 4);
    map.put(4, 4);
    map.put(4, 4);

    print(map);

    return 0;
}

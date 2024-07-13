// Copyright 2024 Severin Denisenko

#include <boost/container/flat_set.hpp>
#include <boost/icl/interval_map.hpp>
#include <fmt/format.h>

#include <cstdint>
#include <iostream>

using number_t       = uint32_t;
using set_t          = boost::container::flat_set<number_t>;
using interval_map_t = boost::icl::interval_map<number_t, set_t>;
using interval_t     = boost::icl::discrete_interval<number_t>;

void print() {
    std::cout << std::endl;
}

void print(interval_map_t& map) {
    for(auto& [interval, set] : map) {
        std::cout << fmt::format("({}: {}): ", interval.lower(), interval.upper());
        for(number_t num : set) {
            std::cout << fmt::format("{} ", num);
        }
        std::cout << std::endl;
    }
}

int main() {
    interval_map_t map;
    map += std::make_pair(interval_t::open(0, 10), set_t {0});
    map += std::make_pair(interval_t::open(5, 10), set_t {1});
    map += std::make_pair(interval_t::open(0, 5), set_t {2});
    map += std::make_pair(interval_t::open(3, 7), set_t {3});

    print(map);

    map -= std::make_pair(interval_t::open(0, 5), set_t {0});
    map -= std::make_pair(interval_t::open(5, 10), set_t {3});

    print();
    print(map);

    return 0;
}

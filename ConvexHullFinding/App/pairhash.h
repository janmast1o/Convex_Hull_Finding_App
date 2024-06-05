#include <iostream>
#include <unordered_map>
#include <utility>

#ifndef PAIR_HASH_H
#define PAIR_HASH_H

struct pair_hash {
    std::size_t operator()(const std::pair<double, double>& p) const {
        auto h1 = std::hash<double>{}(p.first);
        auto h2 = std::hash<double>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};


struct pair_equal {
    bool operator()(const std::pair<double, double>& p1, const std::pair<double, double>& p2) const {
        return p1.first == p2.first && p1.second == p2.second;
    }
};

#endif
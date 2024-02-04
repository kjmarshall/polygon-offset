#pragma once

#include <src/line.h>
#include <ostream>

struct Segment {
    static constexpr float eps = 1e-6f;
    Line line;
    size_t id;
    void print(std::ostream &out) const;
};

bool operator<(Segment const &a, Segment const &b);
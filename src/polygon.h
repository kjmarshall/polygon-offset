#pragma once

#include <ostream>
#include <src/vector2d.h>
#include <vector>

struct CCWPolygon {
    // Assumptions:
    // - closed loop points.front() should equal points.back()
    // - no holes
    CCWPolygon() = default;
    CCWPolygon(std::vector<float> const &coordinates);
    void print(std::ostream &out);
    std::vector<Point2D> points;
    auto begin() { return points.begin(); }
    auto end() { return points.end(); }
};
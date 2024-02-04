#pragma once

#include <src/vector2d.h>

struct Line {
    Point2D p;
    Point2D q;
    float getYPosition(float x, float eps) const;
};

Point2D getIntersection(Line const &l1, Line const &l2, float eps);
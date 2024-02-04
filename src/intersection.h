#pragma once

#include <src/segment.h>
#include <src/vector2d.h>

bool getIntersection(float l1, float r1, float l2, float r2, float eps);
int getOrientation(Point2D const &a, Point2D const &b, Point2D const &c, float eps);
bool getIntersection(Segment const &a, Segment const &b, float eps);
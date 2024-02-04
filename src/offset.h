#pragma once

#include <src/line.h>
#include <src/polygon.h>
#include <vector>

class Offset {

  public:
    explicit Offset(CCWPolygon const &polygon) : _polygon(polygon) {}
    CCWPolygon getOffset(float delta);
    CCWPolygon getOffsetV2(float delta);

  private:
    std::vector<Vector2D> getNormals();
    CCWPolygon getOffsetImpl(float delta);

    CCWPolygon _polygon;
};

float compare(CCWPolygon const &a, CCWPolygon const &b);
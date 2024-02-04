#include "polygon.h"

CCWPolygon::CCWPolygon(std::vector<float> const &coordinates) {
    for (std::size_t i = 0, j = 1; j < coordinates.size(); i += 2, j += 2) {
        points.push_back(Point2D{coordinates[i], coordinates[j]});
    }
    // print(std::cout);
}

void CCWPolygon::print(std::ostream &out) {
    for (auto const &p : points) {
        out << p.x << ", " << p.y << "\n";
    }
}
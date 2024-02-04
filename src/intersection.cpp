#include "intersection.h"

bool getIntersection(float l1, float r1, float l2, float r2, float eps) {
    if (l1 > r1)
        std::swap(l1, r1);
    if (l2 > r2)
        std::swap(l2, r2);
    return std::max(l1, l2) <= std::min(r1, r2) + eps;
}

int getOrientation(Point2D const &a, Point2D const &b, Point2D const &c, float eps) {
    double det = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return std::abs(det) < (eps / 10.f) ? 0 : det > 0 ? 1 : -1;
}

bool getIntersection(Segment const &a, Segment const &b, float eps) {
    return getIntersection(a.line.p.x, a.line.q.x, b.line.p.x, b.line.q.x, eps) &&
           getIntersection(a.line.p.y, a.line.q.y, b.line.p.y, b.line.q.y, eps) &&
           getOrientation(a.line.p, a.line.q, b.line.p, eps) * getOrientation(a.line.p, a.line.q, b.line.q, eps) <= 0 &&
           getOrientation(b.line.p, b.line.q, a.line.p, eps) * getOrientation(b.line.p, b.line.q, a.line.q, eps) <= 0;
}
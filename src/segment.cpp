#include "segment.h"

void Segment::print(std::ostream &out) const {
    out << "p = (" << line.p.x << ", " << line.p.y << "), q = (" << line.q.x << ", " << line.q.y << ")"
        << "\n";
}

bool operator<(Segment const &a, Segment const &b) {
    float x = std::max(std::min(a.line.p.x, a.line.q.x), std::min(b.line.p.x, b.line.q.x));
    return a.line.getYPosition(x, Segment::eps) < b.line.getYPosition(x, Segment::eps) - Segment::eps;
}
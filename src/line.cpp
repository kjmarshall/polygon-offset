#include "line.h"

float Line::getYPosition(float x, float eps) const {
    if (std::abs(p.x - q.x) < eps)
        return p.y;
    float slope = (q.y - p.y) / (q.x - p.x);
    return p.y + slope * (x - p.x);
}

Point2D getIntersection(Line const &l1, Line const &l2, float eps) {
    Vector2D d1 = l1.q.getVector() - l1.p.getVector();
    Vector2D d2 = l2.q.getVector() - l2.p.getVector();
    Vector2D r = l1.p.getVector() - l2.p.getVector();

    float s{0.f};
    // float t{0.f};

    float a = d1.dot(d1);
    float b = d1.dot(d2);
    float c = d1.dot(r);
    float e = d2.dot(d2);
    float f = d2.dot(r);

    float d = a * e - b * b;
    if (d <= eps) {
        // parallel;
        return l1.q;
    } else {
        s = (b * f - c * e) / d;
        // t = (a * f - b * c) / d;
        auto vs = l1.p.getVector() + s * d1;
        // auto vt = l2.p.getVector() + t * d2;
        return {vs.x, vs.y};
    }
}
#include "vector2d.h"

float Vector2D::cross(Vector2D const &other) const { return y * other.x - other.y * x; }
float Vector2D::dot(Vector2D const &other) const { return x * other.x + y * other.y; }
Vector2D Vector2D::normalized() const {
    float denom = 1.f / std::sqrt(x * x + y * y);
    return {x * denom, y * denom};
}

Vector2D operator-(Vector2D const &a, Vector2D const &b) { return {a.x - b.x, a.y - b.y}; }
Vector2D operator+(Vector2D const &a, Vector2D const &b) { return {a.x + b.x, a.y + b.y}; }
Vector2D operator*(Vector2D const &a, float value) { return {a.x * value, a.y * value}; }
Vector2D operator*(float value, Vector2D const &a) { return a * value; }

bool operator==(Point2D const &a, Point2D const &b) { return a.x == b.x && a.y == b.y; }
Vector2D operator-(Point2D const &a, Point2D const &b) { return {a.x - b.x, a.y - b.y}; }
Point2D operator+(Point2D const &a, Vector2D const &b) { return {a.x + b.x, a.y + b.y}; }

Vector2D getNormal(Point2D const &a, Point2D const &b) {
    Vector2D ret;
    if (a == b) {
        return ret;
    }
    auto delta = b.getVector() - a.getVector();
    float denom = 1.f / std::sqrt(delta.x * delta.x + delta.y * delta.y);
    return {delta.y * denom, -delta.x * denom};
}
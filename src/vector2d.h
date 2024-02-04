#pragma once

#include <cmath>

struct Vector2D {
    float x = 0.f;
    float y = 0.f;
    float cross(Vector2D const &other) const;
    float dot(Vector2D const &other) const;
    Vector2D normalized() const;
};

Vector2D operator-(Vector2D const &a, Vector2D const &b);
Vector2D operator+(Vector2D const &a, Vector2D const &b);
Vector2D operator*(Vector2D const &a, float value);
Vector2D operator*(float value, Vector2D const &a);

struct Point2D {
    float x = 0.f;
    float y = 0.f;
    Vector2D getVector() const { return {x, y}; }
};

bool operator==(Point2D const &a, Point2D const &b);
Vector2D operator-(Point2D const &a, Point2D const &b);
Point2D operator+(Point2D const &a, Vector2D const &b);

Vector2D getNormal(Point2D const &a, Point2D const &b);
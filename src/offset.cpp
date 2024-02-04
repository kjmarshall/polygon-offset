#include "offset.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <src/intersection.h>
#include <src/line.h>
#include <src/polygon.h>
#include <src/segment.h>
#include <src/vector2d.h>
#include <vector>

// A simple correction for sharp angles.
Point2D getSphereCorrection(Point2D const &offsetPoint, Point2D const &point, float radius) {
    auto v = (offsetPoint - point).normalized();
    return point + radius * v;
}

Line getOffsetLine(std::vector<Point2D> const &points, std::vector<Vector2D> const &normals, std::size_t i,
                   std::size_t j, float delta) {
    assert(points.size() >= normals.size());
    assert(i < points.size());
    assert(j < points.size());

    auto getPerpPoint = [](Point2D const &pt, Vector2D const &norm, float dist) {
        auto perpVec = pt.getVector() + norm * dist;
        return Point2D{perpVec.x, perpVec.y};
    };
    return Line{getPerpPoint(points[i], normals[i], delta), getPerpPoint(points[j], normals[i], delta)};
}

std::vector<Segment> getSegments(CCWPolygon const &poly) {
    std::vector<Segment> segments;
    size_t id{0};
    for (std::size_t i = 0, j = 1; j < poly.points.size(); ++i, ++j) {
        segments.push_back(Segment{Line{poly.points[i], poly.points[j]}, id++});
    }
    return segments;
}

std::optional<std::pair<size_t, size_t>> getIsPolygonSimpleImpl(CCWPolygon const &poly) {
    struct Event {
        float x;
        int parity;
        int id;
        Event() = delete;
        Event(float x_, int parity_, int id_) : x(x_), parity(parity_), id(id_) {}

        // Sort by smallest x left to right
        bool operator<(Event const &other) {
            if (std::abs(x - other.x) > Segment::eps)
                return x < other.x;
            return parity > other.parity;
        }
    };
    std::vector<Event> events;
    auto segments = getSegments(poly);
    for (std::size_t i = 0; i < segments.size(); ++i) {
        auto &p = segments[i].line.p;
        p.x += Segment::eps;
        p.y += Segment::eps;
        auto const &q = segments[i].line.q;
        events.emplace_back(std::min(p.x, q.x), 1, segments[i].id);
        events.emplace_back(std::max(p.x, q.x), -1, segments[i].id);
    }
    std::sort(events.begin(), events.end());

    std::vector<std::set<Segment>::iterator> lookup(segments.size());
    std::set<Segment> segs;
    auto getPrev = [&segs](auto it) { return it == segs.begin() ? segs.end() : --it; };
    auto getNext = [&segs](auto it) { return ++it; };
    for (std::size_t i = 0; i < events.size(); ++i) {
        auto id = events[i].id;
        if (events[i].parity == 1) {
            auto next = segs.lower_bound(segments[id]);
            auto prev = getPrev(next);
            if (next != segs.end() && getIntersection(*next, segments[id], Segment::eps))
                return std::make_pair(next->id, id);
            if (prev != segs.end() && getIntersection(*prev, segments[id], Segment::eps))
                return std::make_pair(prev->id, id);
            lookup[id] = segs.insert(next, segments[id]);
        } else {
            auto next = getNext(lookup[id]);
            auto prev = getPrev(lookup[id]);
            if (next != segs.end() && prev != segs.end() && getIntersection(*next, *prev, Segment::eps))
                return std::make_pair(prev->id, next->id);
            segs.erase(lookup[id]);
        }
    }
    return std::nullopt;
}

bool getIsPolygonSimple(CCWPolygon const &poly) {
    auto intersectingPair = getIsPolygonSimpleImpl(poly);
    return !intersectingPair;
}

float compare(CCWPolygon const &a, CCWPolygon const &b) {
    float sqError = 0.f;
    assert(a.points.size() == b.points.size());
    auto getError = [](Point2D const &pa, Point2D const &pb) {
        auto errorVec = pb - pa;
        return errorVec.dot(errorVec);
    };
    for (std::size_t i = 0; i < a.points.size(); ++i) {
        sqError += getError(a.points[i], b.points[i]);
    }
    return std::sqrt(sqError);
}

CCWPolygon Offset::getOffset(float delta) { return getOffsetImpl(delta); }

CCWPolygon Offset::getOffsetV2(float delta) {
    auto offset = getOffsetImpl(delta);
    if (!getIsPolygonSimple(offset))
        throw std::runtime_error("Offset polygon is not simple.");
    return offset;
}
std::vector<Vector2D> Offset::getNormals() {
    std::vector<Vector2D> normals;
    auto npts = _polygon.points.size();
    normals.reserve(npts);
    for (std::size_t i = 0, j = 1; j < npts; ++i, ++j) {
        normals.push_back(getNormal(_polygon.points[i], _polygon.points[j]));
    }
    return normals;
}

CCWPolygon Offset::getOffsetImpl(float delta) {
    auto normals = getNormals();
    std::vector<Line> lines;
    for (std::size_t i = 0, j = 1; j < _polygon.points.size(); ++i, ++j) {
        lines.push_back(getOffsetLine(_polygon.points, normals, i, j, delta));
    }
    CCWPolygon offset;
    offset.points.push_back(getIntersection(lines.back(), lines[0], Segment::eps));
    for (std::size_t i = 0, j = 1; j < lines.size(); ++i, ++j) {
        auto const &a = lines[i];
        auto const &b = lines[j];
        offset.points.push_back(getIntersection(a, b, Segment::eps));
    }
    offset.points.push_back(offset.points.front());
    return offset;
}
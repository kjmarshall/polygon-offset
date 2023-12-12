#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <optional>
#include <nlohmann/json.hpp>

static constexpr float eps = 1e-6f;

struct Vector2D
{
    float x = 0.f;
    float y = 0.f;
    float cross(Vector2D const &other) const
    {
        return y * other.x - other.y * x;
    }
    float dot(Vector2D const &other) const
    {
        return x * other.x + y * other.y;
    }
    Vector2D normalized() const
    {
        float denom = 1.f / std::sqrt(x * x + y * y);
        return {x * denom, y * denom};
    }
};

Vector2D operator-(Vector2D const &a, Vector2D const &b)
{
    return {a.x - b.x, a.y - b.y};
}
Vector2D operator+(Vector2D const &a, Vector2D const &b)
{
    return {a.x + b.x, a.y + b.y};
}
Vector2D operator*(Vector2D const &a, float value)
{
    return {a.x * value, a.y * value};
}
Vector2D operator*(float value, Vector2D const &a)
{
    return a * value;
}

struct Point2D
{
    float x = 0.f;
    float y = 0.f;
    Vector2D getVector() const { return {x, y}; }
};

bool operator==(Point2D const &a, Point2D const &b)
{
    return a.x == b.x && a.y == b.y;
}

Vector2D operator-(Point2D const &a, Point2D const &b)
{
    return {a.x - b.x, a.y - b.y};
}

Point2D operator+(Point2D const &a, Vector2D const &b)
{
    return {a.x + b.x, a.y + b.y};
}

Vector2D getNormal(Point2D const &a, Point2D const &b)
{
    Vector2D ret;
    if (a == b)
    {
        return ret;
    }
    auto delta = b.getVector() - a.getVector();
    float denom = 1.f / std::sqrt(delta.x * delta.x + delta.y * delta.y);
    return {delta.y * denom, -delta.x * denom};
};

struct Line
{
    Point2D p;
    Point2D q;
    float getYPosition(float x) const
    {
        if (std::abs(p.x - q.x) < eps)
            return p.y;
        float slope = (q.y - p.y) / (q.x - p.x);
        return p.y + slope * (x - p.x);
    }
};

Point2D getIntersection(Line const &l1, Line const &l2)
{
    Vector2D d1 = l1.q.getVector() - l1.p.getVector();
    Vector2D d2 = l2.q.getVector() - l2.p.getVector();
    Vector2D r = l1.p.getVector() - l2.p.getVector();

    float s{0.f};
    float t{0.f};

    float a = d1.dot(d1);
    float b = d1.dot(d2);
    float c = d1.dot(r);
    float e = d2.dot(d2);
    float f = d2.dot(r);

    float d = a * e - b * b;
    if (d <= eps)
    {
        // parallel;
        return l1.q;
    }
    else
    {
        s = (b * f - c * e) / d;
        t = (a * f - b * c) / d;
        auto vs = l1.p.getVector() + s * d1;
        auto vt = l2.p.getVector() + t * d2;
        return {vs.x, vs.y};
    }
}

bool getIntersection(float l1, float r1, float l2, float r2)
{
    if (l1 > r1)
        std::swap(l1, r1);
    if (l2 > r2)
        std::swap(l2, r2);
    return std::max(l1, l2) <= std::min(r1, r2) + eps;
}

int getOrientation(Point2D const &a, Point2D const &b, Point2D const &c)
{
    double det = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return std::abs(det) < (eps / 10.f) ? 0 : det > 0 ? 1
                                                      : -1;
}

struct Segment
{
    Line line;
    size_t id;
    void print(std::ostream &out) const
    {
        out << "p = (" << line.p.x << ", " << line.p.y << "), q = (" << line.q.x << ", " << line.q.y << ")"
            << "\n";
    }
};

bool operator<(Segment const &a, Segment const &b)
{
    float x = std::max(std::min(a.line.p.x, a.line.q.x), std::min(b.line.p.x, b.line.q.x));
    return a.line.getYPosition(x) < b.line.getYPosition(x) - eps;
}

bool getIntersection(Segment const &a, Segment const &b)
{
    return getIntersection(a.line.p.x, a.line.q.x, b.line.p.x, b.line.q.x) &&
           getIntersection(a.line.p.y, a.line.q.y, b.line.p.y, b.line.q.y) &&
           getOrientation(a.line.p, a.line.q, b.line.p) * getOrientation(a.line.p, a.line.q, b.line.q) <= 0 &&
           getOrientation(b.line.p, b.line.q, a.line.p) * getOrientation(b.line.p, b.line.q, a.line.q) <= 0;
}

// A simple correction for sharp angles.
Point2D
getSphereCorrection(Point2D const &offsetPoint, Point2D const &point, float radius)
{
    auto v = (offsetPoint - point).normalized();
    return point + radius * v;
}

Line getOffsetLine(std::vector<Point2D> const &points, std::vector<Vector2D> const &normals, int i, int j, float delta)
{
    assert(points.size() >= normals.size());
    assert(i >= 0 && i < points.size());
    assert(j >= 0 && j < points.size());

    auto getPerpPoint = [](Point2D const &pt, Vector2D const &norm, float dist)
    {
        auto perpVec = pt.getVector() + norm * dist;
        return Point2D{perpVec.x, perpVec.y};
    };
    return Line{getPerpPoint(points[i], normals[i], delta), getPerpPoint(points[j], normals[i], delta)};
}

struct CCWPolygon
{
    // Assumptions:
    // - closed loop points.front() should equal points.back()
    // - no holes
    CCWPolygon() = default;
    CCWPolygon(std::vector<float> const &coordinates)
    {
        for (auto i = 0, j = 1; j < coordinates.size(); i += 2, j += 2)
        {
            points.push_back(Point2D{coordinates[i], coordinates[j]});
        }
        // print(std::cout);
    }
    void print(std::ostream &out)
    {
        for (auto const &p : points)
        {
            out << p.x << ", " << p.y << "\n";
        }
    }
    std::vector<Point2D> points;
    auto begin() { return points.begin(); }
    auto end() { return points.end(); }
};

std::vector<Segment> getSegments(CCWPolygon const &poly)
{
    std::vector<Segment> segments;
    size_t id{0};
    for (auto i = 0, j = 1; j < poly.points.size(); ++i, ++j)
    {
        segments.push_back(Segment{Line{poly.points[i], poly.points[j]}, id++});
    }
    return segments;
}

std::optional<std::pair<size_t, size_t>> getIsPolygonSimpleImpl(CCWPolygon const &poly)
{
    struct Event
    {
        float x;
        int parity;
        int id;
        Event() = delete;
        Event(float x_, int parity_, int id_) : x(x_), parity(parity_), id(id_) {}

        // Sort by smallest x left to right
        bool operator<(Event const &other)
        {
            if (std::abs(x - other.x) > eps)
                return x < other.x;
            return parity > other.parity;
        }
    };
    std::vector<Event> events;
    auto segments = getSegments(poly);
    for (auto i = 0; i < segments.size(); ++i)
    {
        auto &p = segments[i].line.p;
        p.x += eps;
        p.y += eps;
        auto const &q = segments[i].line.q;
        events.emplace_back(std::min(p.x, q.x), 1, segments[i].id);
        events.emplace_back(std::max(p.x, q.x), -1, segments[i].id);
    }
    sort(events.begin(), events.end());

    std::vector<std::set<Segment>::iterator> lookup(segments.size());
    std::set<Segment> segs;
    auto getPrev = [&segs](auto it)
    {
        return it == segs.begin() ? segs.end() : --it;
    };
    auto getNext = [&segs](auto it)
    {
        return ++it;
    };
    for (auto i = 0; i < events.size(); ++i)
    {
        auto id = events[i].id;
        if (events[i].parity == 1)
        {
            auto next = segs.lower_bound(segments[id]);
            auto prev = getPrev(next);
            if (next != segs.end() && getIntersection(*next, segments[id]))
                return std::make_pair(next->id, id);
            if (prev != segs.end() && getIntersection(*prev, segments[id]))
                return std::make_pair(prev->id, id);
            lookup[id] = segs.insert(next, segments[id]);
        }
        else
        {
            auto next = getNext(lookup[id]);
            auto prev = getPrev(lookup[id]);
            if (next != segs.end() && prev != segs.end() && getIntersection(*next, *prev))
                return std::make_pair(prev->id, next->id);
            segs.erase(lookup[id]);
        }
    }
    return std::nullopt;
}

bool getIsPolygonSimple(CCWPolygon const &poly)
{
    auto intersectingPair = getIsPolygonSimpleImpl(poly);
    return !intersectingPair;
}

class Offset
{

public:
    explicit Offset(CCWPolygon const &polygon) : _polygon(polygon) {}
    CCWPolygon getOffset(float delta)
    {
        return getOffsetImpl(delta);
    }
    CCWPolygon getOffsetV2(float delta)
    {
        auto offset = getOffsetImpl(delta);
        if (!getIsPolygonSimple(offset))
            throw std::runtime_error("Offset polygon is not simple.");
        return offset;
    }

private:
    std::vector<Vector2D> getNormals()
    {
        std::vector<Vector2D> normals;
        auto npts = _polygon.points.size();
        normals.reserve(npts);
        for (auto i = 0, j = 1; j < npts; ++i, ++j)
        {
            normals.push_back(getNormal(_polygon.points[i], _polygon.points[j]));
        }
        return normals;
    }
    CCWPolygon getOffsetImpl(float delta)
    {
        auto normals = getNormals();
        std::vector<Line> lines;
        for (auto i = 0, j = 1; j < _polygon.points.size(); ++i, ++j)
        {
            lines.push_back(getOffsetLine(_polygon.points, normals, i, j, delta));
        }
        CCWPolygon offset;
        offset.points.push_back(getIntersection(lines.back(), lines[0]));
        for (auto i = 0, j = 1; j < lines.size(); ++i, ++j)
        {
            auto const &a = lines[i];
            auto const &b = lines[j];
            offset.points.push_back(getIntersection(a, b));
        }
        offset.points.push_back(offset.points.front());
        return offset;
    }

    CCWPolygon _polygon;
};

float compare(CCWPolygon const &a, CCWPolygon const &b)
{
    float sqError = 0.f;
    assert(a.points.size() == b.points.size());
    auto getError = [](Point2D const &pa, Point2D const &pb)
    {
        auto errorVec = pb - pa;
        return errorVec.dot(errorVec);
    };
    for (auto i = 0; i < a.points.size(); ++i)
    {
        sqError += getError(a.points[i], b.points[i]);
    }
    return std::sqrt(sqError);
}

void test(std::filesystem::path const &inputPath)
{
    using json = nlohmann::json;
    std::ifstream in(inputPath.string());
    json data = json::parse(in);
    float delta = data["offset_distance"];
    std::vector<float> input = data["input_polygon"];
    CCWPolygon inputPoly(input);
    std::filesystem::path outPath("./output");
    {
        auto path = outPath / (inputPath.stem().string() + std::string("-input_polygon.gnuplot"));
        std::ofstream out(path.string());
        inputPoly.print(out);
    }
    std::vector<float> output = data["output_polygon"];
    CCWPolygon expectedPoly(output);
    {
        auto path = outPath / (inputPath.stem().string() + std::string("-output_polygon.gnuplot"));
        std::ofstream out(path.string());
        expectedPoly.print(out);
    }
    Offset off(input);
    auto offPoly = off.getOffset(delta);
    try
    {
        auto offPolyV2 = off.getOffsetV2(delta);
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << std::endl;
    }
    // offPoly.print(std::cout);
    std::ofstream out(outPath / inputPath.stem() / ".gnuplot");
    offPoly.print(out);

    std::cout << "Pointwise sum of errors is: " << compare(expectedPoly, offPoly) << std::endl;
}

int main()
{
    test("./input/circle_m01.json");
    test("./input/circle_p01.json");
    test("./input/flower_p05.json");
    return 0;
}
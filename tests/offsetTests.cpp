#include "paths.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <src/offset.h>

void test(std::filesystem::path const &inputPath) {
    using json = nlohmann::json;
    std::ifstream in(inputPath.string());
    json data = json::parse(in);
    float delta = data["offset_distance"];
    std::vector<float> input = data["input_polygon"];
    CCWPolygon inputPoly(input);
    std::filesystem::path outPath(SOURCE_PATH / "output");
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
    try {
        auto offPolyV2 = off.getOffsetV2(delta);
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    // offPoly.print(std::cout);
    std::ofstream out(outPath / inputPath.stem() / ".gnuplot");
    offPoly.print(out);

    std::cout << "Pointwise sum of errors is: " << compare(expectedPoly, offPoly) << std::endl;
}

TEST(offsetTests, circle_m01) { test(SOURCE_PATH / "input/circle_m01.json"); }
TEST(offsetTests, circle_p01) { test(SOURCE_PATH / "./input/circle_p01.json"); }
TEST(offsetTests, flower_p05) { test(SOURCE_PATH / "./input/flower_p05.json"); }
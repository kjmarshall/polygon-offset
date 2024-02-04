#pragma once
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define SOURCE_PATH std::filesystem::path(TOSTRING(CMAKE_SOURCE_DIR))
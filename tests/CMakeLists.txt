CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)

CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.9.1
    GITHUB_REPOSITORY nlohmann/json
    OPTIONS
    "JSON_BuildTests OFF"
)

include(add-tests)

add_compile_definitions(CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR})
sdk_add_test(offsetTests SOURCES offsetTests.cpp LIBRARIES libOffset nlohmann_json::nlohmann_json)
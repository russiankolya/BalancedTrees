cmake_minimum_required(VERSION 3.29)
project(BalancedTrees)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.12.4
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json httplib)

set(SOURCES
    src/main.cpp
    src/tree_service.cpp
)

add_executable(BalancedTrees ${SOURCES})

target_include_directories(BalancedTrees PRIVATE 
    src
    ${httplib_SOURCE_DIR}
)

target_link_libraries(BalancedTrees PRIVATE
    nlohmann_json::nlohmann_json
)

add_custom_command(TARGET BalancedTrees POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/static
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/static $<TARGET_FILE_DIR:${PROJECT_NAME}>/static
    COMMAND ${CMAKE_COMMAND} -E echo "Copied static files from ${CMAKE_SOURCE_DIR} to $<TARGET_FILE_DIR:${PROJECT_NAME}>/static"
)

add_executable(benchmark src/benchmark/benchmark.cpp)

target_compile_options(benchmark PRIVATE -O3)

target_include_directories(benchmark PRIVATE src)

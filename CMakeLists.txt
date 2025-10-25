cmake_minimum_required(VERSION 4.1.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC
    src/cpp/mesh.hpp
    src/cpp/utils.hpp
    src/cpp/vertex.hpp
    src/cpp/triangle.hpp
    src/cpp/main.cpp
    src/cpp/mesh.cpp
    src/cpp/utils.cpp
    src/cpp/triangle.cpp
    src/cpp/vertex.cpp
)

project(heart VERSION 1.0)
add_executable(heart.out ${SRC})

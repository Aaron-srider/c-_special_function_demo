cmake_minimum_required(VERSION 3.17)
project(c___class_special_function)

set(CMAKE_CXX_STANDARD 14)

# close compile optimization to observe copy and move constructor
add_compile_options(-fno-elide-constructors)

add_executable(c___class_special_function main.cpp)

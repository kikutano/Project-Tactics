# Ensure the minimum required version of CMake is available, otherwise, throw an error
cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

add_subdirectory(Apps)
add_subdirectory(Libs)
add_subdirectory(Engine)

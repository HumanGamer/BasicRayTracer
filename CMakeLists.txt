cmake_minimum_required(VERSION 3.26)
project(BasicRayTracer)

set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(libs/SFML)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.20)

project(string_wave_fdm VERSION 1.0.0 LANGUAGES CXX)

set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(external/eigen EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(string-wave main/main.cpp)
target_compile_features(string-wave PRIVATE cxx_std_17)
target_link_libraries(string-wave PRIVATE Eigen3::Eigen OpenGL::GL GLUT::GLUT)

if(MSVC)
    target_compile_options(string-wave PRIVATE /W4 /permissive-)
else()
    target_compile_options(string-wave PRIVATE -Wall -Wextra -Wpedantic)
endif()

cmake_minimum_required(VERSION 3.20)

project(string_wave_fdm VERSION 1.0.0 LANGUAGES CXX)

include(CTest)

set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(external/eigen EXCLUDE_FROM_ALL)

add_library(string-wave-core src/wave_simulation.cpp)
target_compile_features(string-wave-core PUBLIC cxx_std_17)
target_include_directories(string-wave-core PUBLIC include)
target_link_libraries(string-wave-core PUBLIC Eigen3::Eigen)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(string-wave main/main.cpp src/opengl_visualizer.cpp)
target_include_directories(string-wave PRIVATE include)
target_link_libraries(string-wave PRIVATE string-wave-core OpenGL::GL GLUT::GLUT)

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

enable_project_warnings(string-wave-core)
enable_project_warnings(string-wave)

if(BUILD_TESTING)
    add_executable(string-wave-tests tests/wave_simulation_tests.cpp)
    target_link_libraries(string-wave-tests PRIVATE string-wave-core)
    enable_project_warnings(string-wave-tests)
    add_test(NAME string-wave-tests COMMAND string-wave-tests)
endif()

cmake_minimum_required(VERSION 3.24)
project(ShaderTestLabTests LANGUAGES CXX)

enable_testing()
include(../../cmake/paperboat-cvars.cmake)
add_executable(shader_test_lab_math ShaderTestLabMath.cpp)
target_compile_features(shader_test_lab_math PRIVATE cxx_std_20)
target_include_directories(shader_test_lab_math PRIVATE ../../src)
add_test(NAME shader_test_lab_math COMMAND shader_test_lab_math)

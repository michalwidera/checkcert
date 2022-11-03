cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project (checkcert)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

enable_testing()

set(CONAN_DISABLE_CHECK_COMPILER TRUE)

set(CMAKE_CXX_STANDARD 20)

conan_basic_setup()

add_subdirectory(src)
add_subdirectory(test)
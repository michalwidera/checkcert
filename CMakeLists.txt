cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project (checkcert)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

set(SOURCES_STAGE
    src/main.cpp
    )

add_executable( checkcert ${SOURCES_STAGE} )

target_link_libraries( checkcert ${CONAN_LIBS} -pthread )

set_target_properties(checkcert PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

install(TARGETS checkcert DESTINATION ~/.local/bin)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(checkcert-test)

enable_testing()

set(CMAKE_TEST_DATA_DIR ${CMAKE_SOURCE_DIR}/test/data)

add_test(
  NAME check-cert
  WORKING_DIRECTORY ${CMAKE_TEST_DATA_DIR}
  COMMAND openssl help)
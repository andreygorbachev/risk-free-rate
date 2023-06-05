cmake_minimum_required(VERSION 3.15)

project("risk-free-rate" VERSION 0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(Calendar)
find_package(CouponSchedule)

enable_testing()

add_subdirectory(include)
add_subdirectory(test)

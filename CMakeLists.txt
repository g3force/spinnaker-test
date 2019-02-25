cmake_minimum_required(VERSION 3.10)
project(spinnaker_test)

set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package( SPINNAKER REQUIRED)
add_definitions( -DSPINNAKER )
include_directories( ${SPINNAKER_INCLUDE_DIRS} )
set (SHARED_SRCS ${SHARED_SRCS} ${shared_dir}/capture/capture_spinnaker.cpp)
set (SHARED_HEADERS ${SHARED_HEADERS} ${shared_dir}/capture/capture_spinnaker.h)
message(STATUS "SPINNAKER support enabled")

add_executable(spinnaker_test main.cpp)
target_link_libraries(spinnaker_test ${SPINNAKER_LIBS})
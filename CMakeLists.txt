cmake_minimum_required(VERSION 3.16)
project(ignition-omni1 NONE)

add_custom_target(build ALL ${CMAKE_CURRENT_SOURCE_DIR}/build.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

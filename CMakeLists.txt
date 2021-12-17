cmake_minimum_required(VERSION 3.21)
# FIXTHEM: ign-cmake does not support multiple hypens
project(ignition-omniverse VERSION 0.0.1)

find_package(ignition-cmake2 2.9 REQUIRED)
ign_configure_project()

include(third_party/omni_client_library/CMakeLists.txt)

ign_configure_build()

ign_create_packages()

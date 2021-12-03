cmake_minimum_required(VERSION 3.21)
project(ignition-omniverse-connector)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE)
endif()

include(GNUInstallDirs)
include(third_party/omni_client_library/CMakeLists.txt)

find_package(ignition-plugin1 REQUIRED COMPONENTS register)
find_package(ignition-rendering6 REQUIRED)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cc)

add_library(${PROJECT_NAME} MODULE
  ${sources}
  )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE
  omni_client
  ignition-plugin1::register
  ignition-rendering6
  )

set_target_properties(${PROJECT_NAME} PROPERTIES
  INSTALL_RPATH \${ORIGIN}
  )

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

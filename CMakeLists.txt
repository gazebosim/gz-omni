cmake_minimum_required(VERSION 3.21)
project(ignition-omniverse-connector)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE)
endif()

include(GNUInstallDirs)

function(add_bin_project TARGET URL)
  cmake_parse_arguments(ARG "" EXPECTED_HASH "" ${ARGN})
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/${TARGET})
    file(DOWNLOAD ${URL} ${CMAKE_BINARY_DIR}/download/${TARGET} SHOW_PROGRESS EXPECTED_HASH ${ARG_EXPECTED_HASH})
    file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/download/${TARGET} DESTINATION ${CMAKE_BINARY_DIR}/${TARGET})
  endif()
endfunction()

add_bin_project(nv-usd https://d4i3qtqj3r0z5.cloudfront.net/nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z
  EXPECTED_HASH SHA256=97e0ccfc3d2621ee751439759070ba696720d48127a315133710e19c86271211)
add_bin_project(omni_client_library https://d4i3qtqj3r0z5.cloudfront.net/omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z
  EXPECTED_HASH SHA256=3d62c3ebc7af1a8839c8c066c9fe1670c59802653e63f4452a87d5f536d431ae)
add_bin_project(python https://d4i3qtqj3r0z5.cloudfront.net/python@3.7.9-173.e9ee4ea0-linux-x86_64.7z
  EXPECTED_HASH SHA256=b87e584c57a41cc76c66f85fb7807917f6b82ad535b871f943b734a6c485e35c)

find_package(omni_client REQUIRED PATHS ${CMAKE_SOURCE_DIR}/cmake)
find_package(ignition-plugin1 REQUIRED COMPONENTS register)
find_package(ignition-rendering6 REQUIRED)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cc)

add_library(${PROJECT_NAME} SHARED
  ${sources}
  )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE
  omni_client
  ignition-plugin1::register
  ignition-rendering6
  )

install(TARGETS ${PROJECT_NAME})

find_package(omni_client REQUIRED PATHS ${CMAKE_SOURCE_DIR}/cmake)
find_package(ignition-plugin1 REQUIRED COMPONENTS register)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cc)

add_executable(${PROJECT_NAME}
  ${sources}
  )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE
  omni_client
  )

ign_find_package(ignition-plugin1 REQUIRED COMPONENTS register)
ign_find_package(ignition-rendering6 REQUIRED)

# FIXTHEM: not using ign_get_sources_and_unittests because it does not correctly handle
# incremental builds.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cc)
file(GLOB_RECURSE gtest_sources CONFIGURE_DEPENDS *_TEST.cc)
foreach(s ${gtest_sources})
  list(REMOVE_ITEM sources ${s})
endforeach()

# FIXTHEM: MODULE library is most ideal, but ign-cmake creates a library with no type specified.
ign_create_core_library(SOURCES ${sources} CXX_STANDARD 17)

# FIXTHEM: ign-cmake does not support scope specifiers.
target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  omni_client
  ignition-plugin1::register
  ignition-rendering6
)

# FIXME: See if we need this to allow bundling of the library in omniverse (some bundler automatically adds this).
# set_target_properties(${PROJECT_NAME} PROPERTIES
#   INSTALL_RPATH \${ORIGIN}
# )

ign_build_tests(TYPE UNIT SOURCES ${gtest_sources})

# FIXME: This only works on the build tree.

add_library(omni_client SHARED IMPORTED)

# FIXME: Support windows
set_target_properties(omni_client PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/omni_client_library/release/libomniclient.so)

target_include_directories(omni_client INTERFACE
  ${CMAKE_BINARY_DIR}/nv-usd/include
  ${CMAKE_BINARY_DIR}/omni_client_library/include
  ${CMAKE_BINARY_DIR}/python/include/python3.7m
  )

target_link_directories(omni_client INTERFACE
  ${CMAKE_BINARY_DIR}/nv-usd/lib
  ${CMAKE_BINARY_DIR}/omni_client_library/release
  ${CMAKE_BINARY_DIR}/python/lib
  )

target_compile_definitions(omni_client INTERFACE
  -D_GLIBCXX_USE_CXX11_ABI=0
  )

target_link_libraries(omni_client INTERFACE
  # omni_client_library
  -lar -larch -lgf -ljs -lkind -lpcp -lplug -lsdf -ltf -ltrace -lusd -lusdGeom -lvt -lwork -lusdShade -lusdLux -lomniclient -lpython3.7m -lboost_python37
  ignition-plugin1::register
  )

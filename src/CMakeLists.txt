add_executable(${PROJECT_NAME}
  main.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party/nv-usd/include
  ${CMAKE_SOURCE_DIR}/third_party/omni_client_library/include
  ${CMAKE_SOURCE_DIR}/third_party/python/include/python3.7m
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party/nv-usd/lib
  ${CMAKE_SOURCE_DIR}/third_party/omni_client_library/release
  ${CMAKE_SOURCE_DIR}/third_party/python/lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  -D_GLIBCXX_USE_CXX11_ABI=0
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  -lar -larch -lgf -ljs -lkind -lpcp -lplug -lsdf -ltf -ltrace -lusd -lusdGeom -lvt -lwork -lusdShade -lusdLux -lomniclient -lpython3.7m -lboost_python37
)

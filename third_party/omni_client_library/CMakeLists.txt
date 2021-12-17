# FIXME: Support windows

# FIXME: workaround to test usd while omniclient is not working
find_package(pxr REQUIRED)
add_library(omni_client IMPORTED INTERFACE)
target_link_libraries(omni_client INTERFACE usd usdGeom)

# set(DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/download)
# set(PREBUILT_DIR ${CMAKE_BINARY_DIR}/third_party/omni_client_library/install)

# function(download_library URL FILENAME EXPECTED_HASH EXTRACT_DIR)
#   message(STATUS "Downloading ${FILENAME}...")
#   file(DOWNLOAD ${URL} ${DOWNLOAD_DIR}/${FILENAME} SHOW_PROGRESS EXPECTED_HASH ${EXPECTED_HASH})
#   message(STATUS "Extracting ${FILENAME}...")
#   file(ARCHIVE_EXTRACT INPUT ${DOWNLOAD_DIR}/${FILENAME} DESTINATION ${EXTRACT_DIR})
# endfunction()

# if(NOT EXISTS ${PREBUILT_DIR})
#   download_library(
#     https://d4i3qtqj3r0z5.cloudfront.net/nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z
#     nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z
#     SHA256=97e0ccfc3d2621ee751439759070ba696720d48127a315133710e19c86271211
#     ${PREBUILT_DIR}
#   )
#   download_library(
#     https://d4i3qtqj3r0z5.cloudfront.net/omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z
#     omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z
#     SHA256=3d62c3ebc7af1a8839c8c066c9fe1670c59802653e63f4452a87d5f536d431ae
#     ${DOWNLOAD_DIR}/omni_client_library
#   )
#   file(COPY ${DOWNLOAD_DIR}/omni_client_library/include/ DESTINATION ${PREBUILT_DIR}/include/)
#   file(COPY ${DOWNLOAD_DIR}/omni_client_library/release/ DESTINATION ${PREBUILT_DIR}/lib/)
#   download_library(
#     https://d4i3qtqj3r0z5.cloudfront.net/python@3.7.9-173.e9ee4ea0-linux-x86_64.7z
#     python@3.7.9-173.e9ee4ea0-linux-x86_64.7z
#     SHA256=b87e584c57a41cc76c66f85fb7807917f6b82ad535b871f943b734a6c485e35c
#     ${PREBUILT_DIR}
#   )
# endif()

# add_library(omni_client SHARED IMPORTED)

# set_target_properties(omni_client PROPERTIES IMPORTED_LOCATION ${PREBUILT_DIR}/lib/libomniclient.so)

# target_include_directories(omni_client INTERFACE
#   ${PREBUILT_DIR}/include
#   ${PREBUILT_DIR}/include/python3.7m
#   )

# target_compile_definitions(omni_client INTERFACE
#   -D_GLIBCXX_USE_CXX11_ABI=0
#   )

# target_link_libraries(omni_client INTERFACE
#   ${PREBUILT_DIR}/lib/libar.so
#   ${PREBUILT_DIR}/lib/libarch.so
#   ${PREBUILT_DIR}/lib/libgf.so
#   ${PREBUILT_DIR}/lib/libjs.so
#   ${PREBUILT_DIR}/lib/libkind.so
#   ${PREBUILT_DIR}/lib/libpcp.so
#   ${PREBUILT_DIR}/lib/libplug.so
#   ${PREBUILT_DIR}/lib/libsdf.so
#   ${PREBUILT_DIR}/lib/libtf.so
#   ${PREBUILT_DIR}/lib/libtrace.so
#   ${PREBUILT_DIR}/lib/libusd.so
#   ${PREBUILT_DIR}/lib/libusdGeom.so
#   ${PREBUILT_DIR}/lib/libvt.so
#   ${PREBUILT_DIR}/lib/libwork.so
#   ${PREBUILT_DIR}/lib/libusdShade.so
#   ${PREBUILT_DIR}/lib/libusdLux.so
#   ${PREBUILT_DIR}/lib/libpython3.7m.so
#   ${PREBUILT_DIR}/lib/libboost_python37.so
#   )

# install(DIRECTORY ${PREBUILT_DIR}/lib DESTINATION ${CMAKE_INSTALL_PREFIX})

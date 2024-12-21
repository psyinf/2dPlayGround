file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.3/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=2e89a807e906eab270c68c531129dc6b7f989c79a38a4cca7dde0f3ec93d9c68
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
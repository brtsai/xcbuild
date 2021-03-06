#
# Copyright (c) 2015-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
#

add_library(graphics SHARED
            Sources/Image.cpp
            Sources/PixelFormat.cpp
            Sources/Format/PNG.cpp
            )
target_link_libraries(graphics PUBLIC ext)
target_include_directories(graphics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

find_package(ZLIB REQUIRED)
target_link_libraries(graphics PUBLIC ext "${ZLIB_LIBRARIES}")
target_include_directories(graphics PRIVATE "${ZLIB_INCLUDE_DIR}")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
  find_library(CORE_FOUNDATION CoreFoundation)
  find_library(CORE_GRAPHICS CoreGraphics)
  find_library(IMAGE_IO ImageIO)
  target_link_libraries(graphics PUBLIC "${CORE_FOUNDATION}" "${CORE_GRAPHICS}" "${IMAGE_IO}")
else ()
  set(PNG_STATIC ON)
  find_package(PNG REQUIRED)
  target_link_libraries(graphics PUBLIC "${PNG_LIBRARIES}")
  target_include_directories(graphics PRIVATE "${PNG_INCLUDE_DIRS}")
  target_compile_definitions(graphics PRIVATE "${PNG_DEFINITIONS}")
endif ()

install(TARGETS graphics DESTINATION usr/lib)

if (BUILD_TESTING)
  ADD_UNIT_GTEST(graphics PixelFormat Tests/test_PixelFormat.cpp)
  ADD_UNIT_GTEST(graphics PNG Tests/test_PNG.cpp)
endif ()

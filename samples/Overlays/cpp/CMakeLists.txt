cmake_minimum_required(VERSION 3.26)

# Set the "bin" directory, somewhere in the "ruby" so that it is easier to load.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../ruby/code/ex_hello_c_overlay/bin)

project(SUOverlayExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(WINDOWS_DESKTOP TRUE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(APPLE_MACOS TRUE)
endif ()

SET(SKETCHUP_API_INCLUDE_PATH "" CACHE PATH "SDK include path")
SET(SKETCHUP_API_LIBRARY_PATH "" CACHE PATH "SDK library path")

add_library(ex_hello_overlay MODULE
  exports.hpp
  image2d_overlay.cpp
)

target_include_directories(ex_hello_overlay PRIVATE ${SKETCHUP_API_INCLUDE_PATH})
target_link_directories(ex_hello_overlay PRIVATE ${SKETCHUP_API_LIBRARY_PATH})

if (WINDOWS_DESKTOP)
  target_link_libraries(ex_hello_overlay PRIVATE sketchup)
elseif (APPLE_MACOS)
  # We need the Framework Search Paths pointing to the given SDK include path where
  # SketchUpAPI.framework is located, where we will only load the include files from.
  set(CMAKE_XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS ${SKETCHUP_API_INCLUDE_PATH})
  # We will link with SketchUp executable as the 'Bundle Loader'. Note that we are
  # NOT linking with SketchUpAPI.framework.
  find_program(SketchUpExe "SketchUp" PATHS ${SKETCHUP_API_LIBRARY_PATH})
  target_link_options(ex_hello_overlay PRIVATE LINKER:-bundle -bundle_loader ${SketchUpExe})
endif ()

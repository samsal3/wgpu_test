cmake_minimum_required(VERSION 3.23.1)

project(sf)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# declare the library 
add_library(${PROJECT_NAME})

# features
set_target_properties(${PROJECT_NAME} PROPERTIES 
	C_STANDARD            90
  C_STANDARD_REQUIRED   ON
  C_EXTENSIONS          OFF
  CXX_STANDARD          17 
  CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF)

# add sources
target_sources(${PROJECT_NAME} PRIVATE 
  "sf.c"
  "sf.h"
)

# add includes
target_include_directories(${PROJECT_NAME} PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR})

# compile options
target_compile_options(${PROJECT_NAME} PRIVATE
  "$<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:"
    -Wall
    -Wshadow
    -Wextra
    -Werror
    -Wpedantic
    -pedantic-errors
    -Walloca 
    -Wvla 
    -fdiagnostics-color=always
    -Wno-comment
  ">"
  "$<$<COMPILE_LANG_AND_ID:C,MSVC>:"
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -O0
    -g
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,MSVC>,$<CONFIG:Debug>>:"
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Release>>:"
    -Ofast
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,MSVC>,$<CONFIG:Release>>:"
  ">"
)

# link options
target_link_options(${PROJECT_NAME} PRIVATE
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">")

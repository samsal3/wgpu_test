cmake_minimum_required(VERSION 3.23.1)

project(WebGPUTest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# declare the library 
add_executable(${PROJECT_NAME})

# features
set_target_properties(${PROJECT_NAME} PROPERTIES 
	C_STANDARD            90
  C_STANDARD_REQUIRED   ON
  C_EXTENSIONS          OFF
  CXX_STANDARD          17 
  CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF)

# add sources
add_subdirectory(Source)

# add libraries
add_subdirectory(Vendor)

# add includes
target_include_directories(${PROJECT_NAME} 
  PRIVATE Source)

# compile options
target_compile_options(${PROJECT_NAME} PUBLIC
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
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -O0
    -g
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Release>>:"
    -Ofast
  ">")

# link options
target_link_options(${PROJECT_NAME} PUBLIC
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">")

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE webgpu)

target_copy_webgpu_binaries(${PROJECT_NAME})

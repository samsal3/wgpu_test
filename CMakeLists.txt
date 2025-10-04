cmake_minimum_required(VERSION 3.23.1)

project(wgpu_test)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
add_subdirectory(source)

# add libraries
add_subdirectory(vendor)

# add includes
target_include_directories(${PROJECT_NAME} 
   PRIVATE source)

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
   ">")

# link options
target_link_options(${PROJECT_NAME} PUBLIC
   "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
   -fsanitize=address
   -fsanitize=undefined
   -fsanitize=bounds
   ">")

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
   sf
   webgpu
   glfw
)

target_copy_webgpu_binaries(${PROJECT_NAME})

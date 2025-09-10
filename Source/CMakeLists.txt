add_subdirectory(Common)
add_subdirectory(Rendering)
add_subdirectory(Logic)

target_sources(${PROJECT_NAME} PRIVATE 
  Main.c
)

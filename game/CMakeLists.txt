add_executable(GBjam11
  main.cpp
  game.cpp
  palette.cpp
  controls.cpp
  menu.cpp
)
target_link_libraries(GBjam11 GraphicsEnv ResourceLoader)

if(UNIX) # want linux binary to check current dir for libraries
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath='${ORIGIN}'")
    if(GRAPHICS_BUILD_STATIC)
      target_link_libraries(GBjam11 -static-libgcc -static-libstdc++)
    endif()
  endif()
  target_link_libraries(GBjam11 -pthread)
endif()

add_custom_command(TARGET GBjam11 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../resources"
        $<TARGET_FILE_DIR:GBjam11>)

if(NOT NO_ASSIMP)
  if(NOT BUILD_ASSIMP_STATIC)	
    add_custom_command(TARGET GBjam11 POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      $<TARGET_FILE_DIR:assimp>
      $<TARGET_FILE_DIR:GBjam11>)
  endif()
endif()

if(NOT NO_FREETYPE)
  if(NOT VKENV_BUILD_STATIC)
    add_custom_command(TARGET GBjam11 POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      $<TARGET_FILE_DIR:freetype>
      $<TARGET_FILE_DIR:GBjam11>)
  endif()
endif()

if(NOT VKENV_BUILD_STATIC)
  add_custom_command(TARGET GBjam11 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<TARGET_FILE_DIR:glfw>
    $<TARGET_FILE_DIR:GBjam11>)
endif()

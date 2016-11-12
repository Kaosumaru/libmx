cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(MX_MODULES ${CMAKE_CURRENT_LIST_DIR}/modules)
set(MX_INCLUDES ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/deps/glm )

set(MX_MODULES ${MX_MODULES} PARENT_SCOPE)
set(MX_INCLUDES ${MX_INCLUDES} PARENT_SCOPE)

set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)
include(CommonFunctions)

if (EMSCRIPTEN)
  message("Building for emscripten")

  add_definitions("-std=c++14")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --use-preload-plugins")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --use-preload-plugins" PARENT_SCOPE)

  set(MX_LINK_FLAGS "-s USE_SDL=2 --use-preload-plugins --preload-file ../res/" PARENT_SCOPE)
  set(MX_LIBS MXLib PARENT_SCOPE)
else(EMSCRIPTEN)
  #find_package(GLEW REQUIRED)
  find_package(Boost REQUIRED)
  find_package(SDL2 REQUIRED)

#message("Building for emscripten ${OPENGL_INCLUDE_DIR}")


  set(MX_LINK_FLAGS "" PARENT_SCOPE)
  set(MX_LIBS MXLib ${SDL_LIBRARY}/SDL2.lib ${OPENGL_LIBRARIES} PARENT_SCOPE)
endif(EMSCRIPTEN)



project (MXLib)
option (GAME_RELEASE "Build release?" FALSE)


if (MSVC)
add_definitions( "/MP /W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4503 /nologo" )
configure_msvc_runtime()
endif(MSVC)


ADDFILES(source_list "${PROJECT_SOURCE_DIR}/src")

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${MX_INCLUDES})

add_library(MXLib ${source_list})

##dependencies
set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(bitness 64)
endif()

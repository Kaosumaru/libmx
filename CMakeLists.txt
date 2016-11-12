cmake_minimum_required (VERSION 2.8)

set(MX_MODULES ${CMAKE_CURRENT_LIST_DIR}/modules)
set(MX_INCLUDES ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/deps/glm )

set(MX_MODULES ${MX_MODULES} PARENT_SCOPE)
set(MX_INCLUDES ${MX_INCLUDES} PARENT_SCOPE)

set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)
include(enableC11)
include(CommonFunctions)

find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)

set(MX_LIBS MXLib ${SDL_LIBRARY}/SDL2.lib PARENT_SCOPE)

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

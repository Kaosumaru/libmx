cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(MX_DEFINES -DGLM_PRECISION_MEDIUMP_FLOAT)
set(MX_MODULES ${CMAKE_CURRENT_LIST_DIR}/modules)
set(MX_INCLUDES ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/deps/glm
    ${CMAKE_CURRENT_LIST_DIR}/deps/cpp17_headers/include )

set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)
include(CommonFunctions)

#deps
SET(RPN_USE_JIT OFF CACHE BOOL "Don't use jit")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/RPNParser/RPN" "${CMAKE_CURRENT_BINARY_DIR}/RPN")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/MSL/MSL" "${CMAKE_CURRENT_BINARY_DIR}/MSL")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/litehtml" "${CMAKE_CURRENT_BINARY_DIR}/litehtml")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/soloud" "${CMAKE_CURRENT_BINARY_DIR}/soloud")

set(MX_INCLUDES ${MX_INCLUDES} ${RPN_Includes} ${CMAKE_CURRENT_LIST_DIR}/deps/MSL/MSL ${CMAKE_CURRENT_LIST_DIR}/deps/RPNParser/RPN ${CMAKE_CURRENT_LIST_DIR}/deps/litehtml)
#deps

project (MXLib)
option (GAME_RELEASE "Build release?" FALSE)

ADDFILES(source_list "${PROJECT_SOURCE_DIR}/src")
add_library(MXLib ${source_list})


set(MX_INCLUDES ${MX_INCLUDES} ${SOLOUD_INCLUDE_DIRS})

if (EMSCRIPTEN)
  message("Building for emscripten")
  add_definitions("-std=c++14")

  set(CMAKE_EXECUTABLE_SUFFIX ".html" PARENT_SCOPE)
  # "-s " see https://gitlab.kitware.com/cmake/cmake/issues/15826
  target_compile_options(MXLib PUBLIC "-s" "USE_SDL=2" "-s " "USE_FREETYPE=1" --use-preload-plugins)
  target_link_libraries(MXLib PUBLIC "-s USE_SDL=2" "-s USE_FREETYPE=1"  --use-preload-plugins)
  target_link_libraries(MXLib PUBLIC "--preload-file ${CMAKE_SOURCE_DIR}/res@/res")
else(EMSCRIPTEN)
  find_package(SDL2 REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(Freetype REQUIRED)

  set(MX_INCLUDES ${MX_INCLUDES} ${FREETYPE_INCLUDE_DIRS} ${SDL_INCLUDE_DIR})
  target_link_libraries(MXLib PUBLIC ${SDL_LIBRARY}/SDL2.lib glew32.lib ${OPENGL_LIBRARIES})
endif(EMSCRIPTEN)

if (MSVC)
	add_definitions( "/MP /W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4503 /nologo" )
endif(MSVC)


include_directories(${SDL2_INCLUDE_DIR})
target_compile_definitions(MXLib PUBLIC ${MX_DEFINES})
target_include_directories(MXLib PUBLIC ${MX_INCLUDES})
target_link_libraries(MXLib PUBLIC MSL RPN litehtml soloud ${FREETYPE_LIBRARY})
set(MX_MODULES ${MX_MODULES} PARENT_SCOPE)

cmake_minimum_required(VERSION 2.8)

project(soloud)

#../common/*.cpp ../../src/core/*.cpp ../../src/filter/*.cpp ../../src/backend/sdl2_static/*.cpp ../../src/audiosource/speech/*.c* ../../src/audiosource/wav/*.c* -I ../common -I ../../include
# -s USE_SDL=2 -O2 -D WITH_SDL2_STATIC -s FULL_ES2=1
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

find_package(SDL2 REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/common)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${SDL_INCLUDE_DIR})

set(SOLOUD_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include PARENT_SCOPE)

add_definitions("-D WITH_SDL2_STATIC")


ADDFILES(core_list "${PROJECT_SOURCE_DIR}/src/core")
ADDFILES(filter_list "${PROJECT_SOURCE_DIR}/src/filter")
ADDFILES(backend_list "${PROJECT_SOURCE_DIR}/src/backend/sdl2_static")
ADDFILES(audiosource_speech_list "${PROJECT_SOURCE_DIR}/src/audiosource/speech")
ADDFILES(audiosource_wav_list "${PROJECT_SOURCE_DIR}/src/audiosource/wav")

add_library(soloud STATIC ${core_list} ${filter_list} ${backend_list} ${audiosource_speech_list} ${audiosource_wav_list})

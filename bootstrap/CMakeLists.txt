cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 17)

project (BootstrapProject)
set(APPLICATION_MAJOR_VERSION 0)
set(APPLICATION_MINOR_VERSION 0)
set(APPLICATION_PATCH_VERSION 1)

set(APPLICATION_NAME "Bootstrap")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(EM_HTML_PATH ${CMAKE_CURRENT_LIST_DIR}/html/index.html)
add_definitions(-DMX_DEBUG_RES_PATH="${PROJECT_SOURCE_DIR}/res/")


add_subdirectory(../ "${CMAKE_CURRENT_BINARY_DIR}/MXLib")
set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)


if (MSVC)
    add_definitions( "/DMX_PLATFORM_MSVC  /DUNICODE /D_UNICODE" )
    add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4250 /wd4244 /nologo /Zm180" )
endif(MSVC)

set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)
add_definitions("${MX_DEFINES}")
include_directories(${PROJECT_SOURCE_DIR})

add_executable(BootstrapProject main.cpp TestManager.cpp TestManager.h ${SRCS})
target_link_libraries(BootstrapProject MXLib)

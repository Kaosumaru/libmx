cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

project (BootstrapProject)

add_subdirectory(../ "${CMAKE_CURRENT_BINARY_DIR}/MXLib")
set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)


if (MSVC)
configure_msvc_runtime()
add_definitions( "/DMX_PLATFORM_MSVC  /DUNICODE /D_UNICODE" )
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4250 /wd4244 /nologo /Zm180" )
endif(MSVC)

if (EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  add_definitions("-std=c++14")
endif(EMSCRIPTEN)

#ADDFILES(source_list "${PROJECT_SOURCE_DIR}/src")
include_directories(${MX_INCLUDES})

add_executable(BootstrapProject main.cpp ${SRCS})
target_link_libraries(BootstrapProject ${MX_LIBS})
set_target_properties(BootstrapProject PROPERTIES LINK_FLAGS "${MX_LINK_FLAGS}")

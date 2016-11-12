cmake_minimum_required (VERSION 2.8)

project (BootstrapProject)

add_subdirectory(../ "${CMAKE_CURRENT_BINARY_DIR}/MXLib")
set(CMAKE_MODULE_PATH ${MX_MODULES} CACHE TYPE STRING FORCE)


if (MSVC)
configure_msvc_runtime()
add_definitions( "/DMX_PLATFORM_MSVC  /DUNICODE /D_UNICODE" )
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4250 /wd4244 /nologo /Zm180" )
endif(MSVC)

#ADDFILES(source_list "${PROJECT_SOURCE_DIR}/src")
include_directories(${MX_INCLUDES})

add_executable(BootstrapProject main.cpp ${SRCS})
target_link_libraries(BootstrapProject ${MX_LIBS})

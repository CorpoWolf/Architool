cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

set(PROJECT_NAME architool)
project(${PROJECT_NAME})

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(SOURCE_FILES
    app.cpp
)

set(LIBRARIES
    OpenMaya Foundation
)

build_plugin()
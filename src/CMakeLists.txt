cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /EHsc")

set(PROJECT_NAME architool)
project(${PROJECT_NAME})

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(SOURCE_FILES
	mpmain.cpp
	./ArchiWalls/wallGen.cpp
	./ArchiWalls/wallModOpen.cpp
	./imprLib/imprMath.cpp
	./imprLib/imprString.cpp
)

set(LIBRARIES
	OpenMaya Foundation
)

build_plugin()
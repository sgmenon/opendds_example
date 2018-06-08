cmake_minimum_required(VERSION 3.1)

project(opendds_example VERSION 1.0.0 LANGUAGES CXX)

#set(DEBUG_SUFFIX d)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
include(${PROJECT_SOURCE_DIR}/cmake/dds_utils.cmake)
find_package(DDS 4.5 REQUIRED)

## Set this externally using -DBOOST_ROOT=C:/boost/boost_1_64_0 BOOST_INCLUDEDIR=c:/boost/boost_1_64_0 BOOST_LIBRARYDIR=C:/boost/boost_1_64_0/stage/lib
#add_definitions( -DBOOST_ALL_NO_LIB )
#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#find_package(Boost REQUIRED regex system timer thread date_time signals filesystem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("${CMAKE_SOURCE_DIR}/src/idl_generated" 
${DDS_INCLUDE_DIRS}
#${Boost_INCLUDE_DIRS}
)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libraries"
"ENV{ACE_ROOT}/lib"
"ENV{DDS_ROOT}/lib"
)

add_subdirectory(src)

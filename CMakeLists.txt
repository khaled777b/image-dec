cmake_minimum_required (VERSION 2.6)
project (ImageDEC)

find_package( Boost )

set( SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp" )

include_directories( "${Boost_INCLUDE_DIR}" )

add_executable( image-dec ${SRCS} )

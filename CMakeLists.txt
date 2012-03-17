cmake_minimum_required (VERSION 2.6)
project (ImageDEC)

find_package( Boost )
find_package( SDL )

include_directories(
  "${Boost_INCLUDE_DIR}"
  "${SDL_INCLUDE_DIR}"
  )

set( SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp" )

set( LIBS "${SDL_LIBRARY}" )

add_executable( image-dec ${SRCS} )

target_link_libraries( image-dec "${LIBS}" )
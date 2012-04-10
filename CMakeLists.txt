cmake_minimum_required (VERSION 2.6)
project (ImageDEC)

find_package( Boost )
find_package( SDL )

include_directories(
  "${Boost_INCLUDE_DIR}"
  "${SDL_INCLUDE_DIR}"
  )

link_directories(
  "${Boost_LIBRARY_DIRS}"
  )

add_executable( image-dec
   ${PROJECT_SOURCE_DIR}/src/main.cpp
   )

target_link_libraries( image-dec
  boost_program_options
  ${SDL_LIBRARY}
  tiff jpeg png
  )

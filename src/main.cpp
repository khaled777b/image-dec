/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "image_types.hpp"
#include "image_io.hpp"
#include "image_vector.hpp"
#include "image_sdl.hpp"

namespace opts = boost::program_options;

int main( int argc, char* argv[] )
{
  // Image variables
  std::string input_path = "";

  // Display variables
  bool   display = false;
  Screen screen  = NULL;
  
  opts::options_description options( "Usage:" );
  options.add_options()
    ( "help", "Print usage information" )
    ( "input,i", opts::value<std::string>(),
      "Path to input image." )
    ( "display,d", "Display image." );
  
  opts::variables_map vm;
  opts::store( opts::parse_command_line( argc, argv, options ), vm );
  opts::notify( vm );

  if( vm.count("help") )
    {
      std::cout << "image_dec " << options << std::endl;
      return 1;
    }

  std::cout << "=======  ImageDEC =======\n";

  if( vm.count("input") ) 
    input_path = vm[ "input" ].as<std::string>();

  if( vm.count("display") )
    display = initializeSDL();

  image_t input_image;  
  if( input_path.length() > 0 )
    {
      std::cout << " * Reading input image: " << input_path << std::endl;
      read_image( input_path, input_image );
  
      std::cout << "   - " 
                << input_image.num_channels() << " channel, "
                << input_image.width() << " x "
                << input_image.height() << " successfully read.\n";

      vector_t v = nth_channel_vector( input_image, 0 );
    }

  if( display )
    {
      screen = openImageWindow( input_image );
      draw_image( input_image, screen );
    }


  SDL_Event event;
  
  while( display )
    {
      SDL_PollEvent( &event );

      switch( event.type ) {
      case SDL_KEYDOWN:
        switch( event.key.keysym.sym )
          {
          case SDLK_q:
          case SDLK_ESCAPE:
            display = false;
            break;
          }
        break;
      case SDL_QUIT:
        display = false;
        break;
      }
    }
    
  return 0;
}

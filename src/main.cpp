/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "image_types.hpp"
#include "image_io.hpp"

namespace opts = boost::program_options;

int main( int argc, char* argv[] )
{
  std::string input_path = "";
  
  opts::options_description options( "Usage:" );
  options.add_options()
    ( "help", "Print usage information" )
    ( "input,i", opts::value<std::string>(),
      "Path to input image." );
  
  opts::variables_map vm;
  opts::store( opts::parse_command_line( argc, argv, options ), vm );
  opts::notify( vm );

  if( vm.count("help") )
    {
      std::cout << "image_dec " << options << std::endl;
      return 1;
    }

  if( vm.count("input") ) 
    input_path = vm[ "input" ].as<std::string>();

  std::cout << "=======  ImageDEC =======\n";

  image_t input_image;  
  if( input_path.length() > 0 )
    {
      std::cout << " * Reading input image: " << input_path << std::endl;
      read_image( input_path, input_image );
  
      std::cout << "   - " 
                << input_image.num_channels() << " channel, "
                << input_image.width() << " x "
                << input_image.height() << " successfully read.\n";
    }
  
  return 0;
}

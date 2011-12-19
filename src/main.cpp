/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "image_types.hpp"
#include "image_io.hpp"
#include "image_vector.hpp"
#include "image_grad.hpp"

namespace opts = boost::program_options;

int main( int argc, char* argv[] )
{
  // input
  std::string input_path = "";
  
  // gradient computation
  bool compute_grad = false;
  std::string grad_x_path = "";
  std::string grad_y_path = "";

  opts::options_description options( "Usage:" );
  options.add_options()
    ( "help", "Print usage information" )
    ( "input,i", opts::value<std::string>(),
      "Path to input image." )
    ( "grad,g",
      "Compute the gradient of the image." );
  
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

  if( vm.count("grad") ) 
    {
      compute_grad = true;

      size_t pos = input_path.find_last_of( '.' );

      grad_x_path = input_path.substr( 0, pos )
        + "_dx" + input_path.substr( pos );

      grad_y_path = input_path.substr( 0, pos )
        + "_dy" + input_path.substr( pos );
    }

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

  if( compute_grad )
    {
      const size_t w = input_image.width();
      const size_t h = input_image.height();
      
      image_grad_op grad( w, h );
      
      vector_t u  = nth_channel_vector( input_image, 0 );
      vector_t du = grad * u;
      
      using mtl::irange;

      const size_t mid = ( w-1 ) * h;
      const size_t end = du.size();
      
      vector_t u_x = du[ irange( 0, mid ) ];
      vector_t u_y = du[ irange( mid, end ) ];
      
      gray8_image_t du_x( w-1, h );
      gray8_image_t du_y( w, h-1 );
      
      
    }
  
  return 0;
}

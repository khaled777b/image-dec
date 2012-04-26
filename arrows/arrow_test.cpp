#include <iostream>

#include <boost/type_traits.hpp>
#include <boost/proto/proto_typeof.hpp>

#include "arrow.hpp"
#include "simple_launcher.hpp"

using namespace arrows;

const char* hello() 
{
  // std::cout << "hello ";
  return "hello";
}

const char* world() 
{
  // std::cout << "world!";
  return "world";
}

std::string concat( const char* s1, const char* s2 )
{
  return std::string( s1 ) + " " + std::string( s2 );
}

void print( const std::string& s )
{
  std::cout << s << std::endl;
}

int main()
{
  launch_simple( ( arr( hello ), arr( world ) )
                 >> arr( concat )
                 >> arr( print ) )();

  // auto hello_arrow = ( arr( hello ), arr( world ) ) >> arr( concat ) >> arr( print );

  // launch_simple( hello_arrow )();

  // What I'd like to be able to do:

  // ui::file_chooser   choose_file1, choose_file2;
  // image::reader      read_image1, read_image2;
  // image::convolution convolve;
  // image::viewer      view;
  
  // auto prog = ( choose_file1, choose_file2 ) >> ( read_image1, read_image2 ) >> convolve >> view;
      
  return 0;
}

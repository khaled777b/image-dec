#include <iostream>

#include <boost/proto/proto_typeof.hpp>

#include "arrow.hpp"
#include "simple_launcher.hpp"

using namespace arrows;


int test_fun1( void ) 
{
  return 1;
}

const char* test_fun2( int x ) 
{
  return "hello";
}

const char* hello() 
{
  return "hello";
}

const char* world() 
{
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
  // BOOST_PROTO_AUTO( a, launch_simple( arr( test_fun1 )
  //                                     >> arr( test_fun2 ) ) );

  fusion::vector< const char* > hello =
    launch_simple( arr( test_fun1 ) >> arr( test_fun2 ) )();
  
  std::cout << fusion::front( hello ) << std::endl;

  // launch_simple( ( hello, world ) >> concat >> print );

  return 0;
}

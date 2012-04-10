/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_IO_HPP
#define IMAGE_DEC_IMAGE_IO_HPP

#include <string>

#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <boost/gil/extension/io/tiff_dynamic_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

#include "image_types.hpp"

static const char* jpeg_ext[] =  {
  "jpg", "jpeg", "JPG", "JPEG", NULL
};

static const char* png_ext[] =  {
  "png", "PNG", NULL
};

static const char* tiff_ext[] =  {
  "tif", "tiff", "TIF", "TIFF", NULL
};

struct bad_file_type :
  public std::exception
{
  std::string m_ext;

  bad_file_type( const std::string& ext ) :
    m_ext( ext )
  {}

  virtual ~bad_file_type() throw() {}

  virtual const char* what() const throw()
  {
    std::string w = "Unrecognized file type: " + m_ext;
    return w.c_str();
  }
};

bool is_ext( const std::string& ext, const char* ext_list[] )
{
  size_t n = 0;
  while( ext_list[n] )
    if( ext == std::string( ext_list[n++] ) )
      return true;
  return false;
}

void split_path( const std::string& path, 
                 std::string& dir, 
                 std::string& filename,
                 std::string& ext )
{
  size_t p0 = path.find_last_of( "/\\" );
  dir = path.substr( 0, p0 );
  
  size_t p1 = path.find_last_of( "." );
  filename = path.substr( p0+1, p1 );
  
  ext = path.substr( p1+1 );
}

void read_image( const std::string& path, image_t& image )
{
  std::string dir, filename, ext;
  
  split_path( path, dir, filename, ext );
  
  if( is_ext( ext, jpeg_ext ) )
    {
      boost::gil::jpeg_read_image( path, image );
    }
  else if( is_ext( ext, tiff_ext ) )
    {
      boost::gil::tiff_read_image( path, image );
    }
  else if( is_ext( ext, png_ext ) )
    {
      boost::gil::png_read_image( path, image );
    }
  else
    {
      throw bad_file_type( ext );
    }
}

template< typename Image >
void write_image( const std::string& path, Image& image )
{
  std::string dir, filename, ext;
  
  split_path( path, dir, filename, ext );
  
  if( is_ext( ext, jpeg_ext ) )
    {
      boost::gil::jpeg_write_view( path, view( image ) );
    }
  else if( is_ext( ext, tiff_ext ) )
    {
      boost::gil::tiff_write_view( path, view( image ) );
    }
  else if( is_ext( ext, png_ext ) )
    {
      boost::gil::png_write_view( path, view( image ) );
    }
  else
    {
      throw bad_file_type( ext );
    }
}

#endif

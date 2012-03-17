/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_GRAD_HPP
#define IMAGE_DEC_IMAGE_GRAD_HPP

#include <boost/numeric/mtl/mtl.hpp>

typedef mtl::compressed2D< double > grad_op;

struct image_grad_op :
  public grad_op
{
  image_grad_op( size_t w, size_t h ) :
    grad_op( (w-1)*h + w*(h-1), w*h )
  {
    mtl::matrix::inserter< grad_op > D( *this, 2 );
    
    // First we compute the horizontal components of the gradient.
    size_t i = 0;
    for( size_t y = 0; y < h; ++y )
      for( size_t x = 1; x < w; ++x )
        {
          const size_t j1 = x + y * h;
          const size_t j0 = j1 - 1;
          
          D( i, j1 ) <<  1.0;
          D( i, j0 ) << -1.0;
          
          ++i;
        }

    for( size_t y = 1; y < h; ++y )
      for( size_t x = 0; x < w; ++x )
        {
          const size_t j1 = x + y * h;
          const size_t j0 = j1 - h;
          
          D( i, j1 ) <<  1.0;
          D( i, j0 ) << -1.0;
          
          ++i;
        }
  }  
  
};


#endif

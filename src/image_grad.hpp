/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_GRAD_HPP
#define IMAGE_DEC_IMAGE_GRAD_HPP

#include <boost/numeric/mtl/mtl.hpp>


struct image_grad_op
{
  typedef mtl::compressed2D< double > grad_op;
  typedef unsigned long size_type;
  
  image_grad_op( size_type w, size_type h ) :
    m_op( (w-1)*h + w*(h-1), w*h )
  {
    mtl::matrix::inserter< grad_op > D( m_op, 2 );
    
    // First we compute the horizontal components of the gradient.
    size_type i = 0;
    for( size_type y = 0; y < h; ++y )
      for( size_type x = 1; x < w; ++x )
        {
          const size_type j1 = x + y * h;
          const size_type j0 = j1 - 1;
          
          D( i, j1 ) <<  1.0;
          D( i, j0 ) << -1.0;
          
          ++i;
        }

    // Then we compute the vertical components of the gradient.
    for( size_type y = 1; y < h; ++y )
      for( size_type x = 0; x < w; ++x )
        {
          const size_type j1 = x + y * h;
          const size_type j0 = j1 - h;
          
          D( i, j1 ) <<  1.0;
          D( i, j0 ) << -1.0;
          
          ++i;
        }
  }  

  const grad_op& operator() () const 
  {
    return m_op;
  }
private:
  grad_op m_op;
};


#endif

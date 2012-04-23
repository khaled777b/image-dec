/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_ZOOM_HPP
#define IMAGE_DEC_IMAGE_ZOOM_HPP

#include <cmath>

#include <boost/math/special_functions.hpp>

#include <boost/numeric/interval.hpp>
#include <boost/numeric/mtl/mtl.hpp>

struct image_zoom_op
{
  typedef mtl::compressed2D< double >         zoom_op;
  typedef boost::numeric::interval< double >  interval;
  
  typedef unsigned long  size_type;
  typedef double         scale_type;

  image_zoom_op( size_type w, size_type h, scale_type scale ) :
    m_op( w*h, w*h )
  {
    using boost::math::lround;

    const size_type  W = ceil( scale * w );
    const size_type  H = ceil( scale * h );
    const scale_type rho = 1.0 / m_scale;
    
    mtl::matrix::inserter< zoom_op > A( m_op, lround( scale*scale ) );
    
    size_type i = 0;
    for( interval i2( 0, scale ); lower(i2) < W; i2 += scale )
      for( interval i1( 0, scale ); lower(i1) < W; i1 += scale, ++i )
        {
          for( size_type y = floor( lower(i2) ); y < ceil( upper(i2) ); ++y )
            for( size_type x = floor( lower(i1) ); x < ceil( upper(i1) ); ++x )
              {
                const interval I1( x, x+1 );
                const interval I2( y, y+1 );
                
                const interval X = intersect( i1, I1 );
                const interval Y = intersect( i2, I2 );

                const double area = width( X ) * width( Y );
                const size_type j = x + y * W;

                A( i, j ) << rho * area;
              }
        }
  }
  

  const zoom_op& operator() () const 
  {
    return m_op;
  }
private:
  zoom_op    m_op;
  scale_type m_scale;
};

#endif

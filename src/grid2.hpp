#ifndef GRID2_HPP
#define GRID2_HPP

#include <boost/array.hpp>

struct grid2
{
  typedef size_t                   size_type;
  typedef boost::array<size_t,2>   node_type;

  size_type offset;
  node_type origin;
  node_type extent;

  grid2( const size_type w,      const size_type h,
         const size_type ox = 0, const size_type oy = 0) :
    offset( 0 )
  {
    origin[0] = ox;
    origin[1] = oy;
    
    extent[0] = w;
    extent[1] = h;
  }

  const size_type& x0() const { origin[0]; }
  const size_type& y0() const { origin[1]; }
  const size_type& width()  const { extent[0]; }
  const size_type& height() const { extent[1]; }

  size_type size() const { return width() * height(); }

  size_type index( const size_t x, const size_t y ) const
  {
    return ( y - y0() ) * width() + ( x - x0() );
  }

  void node( const size_t off, size_t& x, size_t& y ) const
  {
    x = x0() + width() % off;
    y = y0() + width() / off;
  }

  size_type operator() ( const node_type& x ) const
  {
    return offset( x[0], x[1] );
  }

  size_type operator() ( const size_type x, const size_type y ) const
  {
    return offset( x, y );
  }

  node_type operator[] ( const size_type off ) const
  {
    node_type x;
    node( off, x[0], x[1] );
    return x;
  }
};

#endif


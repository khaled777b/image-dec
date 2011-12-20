#ifndef GRID_DISSECTION_ORDER_HPP
#define GRID_DISSECTION_ORDER_HPP

#include <grid2.hpp>

template< typename Vector >
void grid2_dissection_order( const grid2& g0,
                             Vector& perm )
{
  typedef grid2::size_type size_type;
  typedef grid2::node_type node_type;
  
  const size_t width  = g0.width();
  const size_t height = g0.height();
  const size_t size   = width * height;
  
  node_type x;
  size_type mid, i;

  grid2 g1( g0 );
  grid2 g2( g0 );
  grid2 gs( g0 );

  if( width >= 4 || height >= 4 ) {
#ifdef DEBUG_DISSECTION_ORDER
    printf( "dissecting %dx%d grid based at %d:\n", width, height, g0.base ); 
#endif
    if( width < height ) {
      mid = height / 2;

      // 1st grid
      g1.height() = g0.y0() + mid-1;

      // 2nd grid
      g2.offset += g1.size();
      g2.y0()   += mid-1;
      
      // Separating grid
      gs.offset   = g2.offset + g2.size();
      gs.y0()     = g0.y0() + mid-1;
      gs.height() = g0.y0() + mid+1;

#ifdef DEBUG_DISSECTION_ORDER
      printf( "\tg1: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              g1.offset, g1.x0(), g1.width(), 
              g1.y0(), g1.height() ); 
      printf( "\tgs: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              gs.offset, gs.x0(), gs.width(), 
              gs.y0(), gs.height() ); 
      printf( "\tg2: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              g2.offset, g2.x0(), g2.width(), 
              g2.y0(), g2.height() ); 
      printf( "\n" );
      printf( "Mapping Nodes:\n" );
#endif

      for( i = g0.offset(); i < size; ++i )
        {
          x = g0[i];

#ifdef DEBUG_DISSECTION_ORDER
          printf( "\tNode %d @ (%d,%d) -> ", i, x[0], x[1] );
          if( x[1] >= g2.y0() ) printf("%d (g2)\n", g2(x));
          else if( x[1] >= gs.y0() ) printf("%d (gs)\n", gs(x));
          else printf("%d (g1)\n", g1(x));
#endif
          if( x[1] >= g2.y0() ) { p[i] = g2(x); continue; }
          if( x[1] >= gs.y0() ) { p[i] = gs(x); continue; }
        
          p[i] = g1(x);
        } // for( i, p )
      
      grid_dissection_order( g1, perm );
      grid_dissection_order( g2, perm );
    } else { // width >= height
      mid = width / 2;

      // 1st grid
      g1.width() = g0.x0() + mid-1;

      // 2nd grid
      g2.offset += g1.size();
      g2.x0()   += mid+1;
      
      // Separating grid
      gs.offset  = g2.offset + g2.size();
      gs.x0()    = g0.x0() + mid-1;
      gs.width() = g0.x0() + mid+1;

#ifdef DEBUG_DISSECTION_ORDER
      printf( "\tg1: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              g1.offset, g1.x0(), g1.width(), 
              g1.y0(), g1.height() ); 
      printf( "\tgs: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              gs.offset, gs.x0(), gs.width(), 
              gs.y0(), gs.height() ); 
      printf( "\tg2: offset = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
              g2.offset, g2.x0(), g2.width(), 
              g2.y0(), g2.height() ); 
      printf( "\n" );
      printf( "Mapping Nodes:\n" );
#endif

      for( i = g0( gs.origin ); i < size; ++i )
        {
          x = g0[i];
#ifdef DEBUG_DISSECTION_ORDER
          printf( "\tNode %d @ (%d,%d) -> ", i, x[0], x[1] );
          if( x[0] >= g2.x0() ) printf("%d (g2)\n", g2(x));
          else if( x[0] >= gs.x0() ) printf("%d (gs)\n", gs(x));
          else printf("%d (g1)\n", *p);
#endif
	
          if( x[0] >= g2.x0() ) { p[i] = g2(x); continue; }
          if( x[0] >= gs.x0() ) { p[i] = gs(x); continue; }

          p[i] = g1(x);
      } // for( i, p )
      
      grid_dissection_order( g1, perm );
      grid_dissection_order( g2, perm );
    } // if( width < height )
  }
}

#endif



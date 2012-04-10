
#include <grid2.hpp>

perm_t grid_row_order( const grid2& g, const perm_t& col_perm )
{
  typedef grid2::node_type node;
  
  const size_t N = size( col_perm );
  
  perm_t p( g.num_edges() );
  perm_t q = inverse_perm( col_perm );

  for( size_t i = 0, j = 0; i < N; ++i )
    {
      node n = g[ q[i] ];
    
      if( g.in_bounds( n[0]-1, n[1] ) ) 
        {
          p[ g.hedge( n[0]-1, n[1] ) ] = j++;
        }
      
      if( g._in_bounds( n[0], n[1]-1 ) )
        {
          p[ g.vedge( n[0], n[1]-1 ) ] = j++;
        }
    }
  
  return p;
}

// Column major grid dissection order for the sparse 
// QR decomposition algorithm.
perm_t grid_dissection_order( const grid2& g0 )
{
  const size_t w  = g.width();
  const size_t h  = g.height();
  const size_t x0 = g.x0();
  const size_t y0 = g.y0();
  
  int mid, x, y, i, *p;
  grid2 g1, g2, gs;
  g1 = g2 = gs = *g0;

  if( w >= 4 || h >= 4 ) 
    {
      if( w < h )
        {
          size_t mid = h / 2;

          // 1st grid
          grid2 g1( w, mid-1, x0, y0 );

          // 2nd grid
          grid2 
          g2.base = g0->base + Size(g1);
          g2.ran[1][0] = g0->ran[1][0] + mid+1;
          
          // Separating grid
          gs.base = g2.base + Size(g2);
          gs.ran[1][0] = g0->ran[1][0] + mid-1;
          gs.ran[1][1] = g0->ran[1][0] + mid+1;

#ifdef DEBUG_DISSECTION_ORDER
      printf( "\tg1: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      g1.base, g1.ran[0][0], g1.ran[0][1], 
	      g1.ran[1][0], g1.ran[1][1] ); 
      printf( "\tgs: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      gs.base, gs.ran[0][0], gs.ran[0][1], 
	      gs.ran[1][0], gs.ran[1][1] ); 
      printf( "\tg2: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      g2.base, g2.ran[0][0], g2.ran[0][1], 
	      g2.ran[1][0], g2.ran[1][1] ); 
      printf( "\n" );
      printf( "Mapping Nodes:\n" );
#endif

      for( i=g0->base, p=perm+i; i < size; ++i, ++p ) {
	Node(*g0,i,x,y);	
#ifdef DEBUG_DISSECTION_ORDER
	printf( "\tNode %d @ (%d,%d) -> ", i, x, y );
	if( y >= g2.ran[1][0] ) printf("%d (g2)\n", Index(g2,x,y));
	else if( y >= gs.ran[1][0] ) printf("%d (gs)\n", Index(gs,x,y));
	else printf("%d (g1)\n", Index(g1,x,y));
#endif
	if( y >= g2.ran[1][0] ) {*p = Index(g2,x,y); continue;}
	if( y >= gs.ran[1][0] ) {*p = Index(gs,x,y); continue;}

	*p = Index(g1,x,y);
      } // for( i, p )
      
      grid_dissection_order( &g1, perm );
      grid_dissection_order( &g2, perm );
    } else { // width >= height
      mid = width / 2;

      // 1st grid
      g1.ran[0][1] = g0->ran[0][0] + mid-1;

      // 2nd grid
      g2.base = g0->base + Size(g1);
      g2.ran[0][0] = g0->ran[0][0] + mid+1;
      
      // Separating grid
      gs.base = g2.base + Size(g2);
      gs.ran[0][0] = g0->ran[0][0] + mid-1;
      gs.ran[0][1] = g0->ran[0][0] + mid+1;

#ifdef DEBUG_DISSECTION_ORDER
      printf( "\tg1: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      g1.base, g1.ran[0][0], g1.ran[0][1], 
	      g1.ran[1][0], g1.ran[1][1] ); 
      printf( "\tgs: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      gs.base, gs.ran[0][0], gs.ran[0][1], 
	      gs.ran[1][0], gs.ran[1][1] ); 
      printf( "\tg2: base = %d,   \tran_x = [%d,%d),   \tran_y = [%d,%d)\n", 
	      g2.base, g2.ran[0][0], g2.ran[0][1], 
	      g2.ran[1][0], g2.ran[1][1] ); 
      printf( "\n" );
      printf( "Mapping Nodes:\n" );
#endif

      i = Index(*g0,gs.ran[0][0],gs.ran[1][0]);
      for( p=perm+i; i < size; ++i, ++p ) {
	Node(*g0,i,x,y);
#ifdef DEBUG_DISSECTION_ORDER
	printf( "\tNode %d @ (%d,%d) -> ", i, x, y );
	if( x >= g2.ran[0][0] ) printf("%d (g2)\n", Index(g2,x,y));
	else if( x >= gs.ran[0][0] ) printf("%d (gs)\n", Index(gs,x,y));
	else printf("%d (g1)\n", *p);
#endif
	
	if( x >= g2.ran[0][0] ) {*p = Index(g2,x,y); continue;}
	if( x >= gs.ran[0][0] ) {*p = Index(gs,x,y); continue;}
      } // for( i, p )
      
      grid_dissection_order( &g1, perm );
      grid_dissection_order( &g2, perm );
    } // if( width < height )
  }
}

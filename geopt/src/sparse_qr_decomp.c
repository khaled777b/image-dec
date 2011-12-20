#include <stdio.h>
#include <blas.h>
#include <cblas.h>

#include "sparse_qr_decomp.h"

// #define DEBUG_DISSECTION_ORDER

// Column major grid dissection order for the sparse 
// QR decomposition algorithm.
void grid_dissection_order( const grid2 *g0, int *perm )
{
  int width = Width(*g0);
  int height = Height(*g0);
  int size = width*height;

  int mid, x, y, i, *p;
  grid2 g1, g2, gs;
  g1 = g2 = gs = *g0;

  if( width >= 4 || height >= 4 ) {
#ifdef DEBUG_DISSECTION_ORDER
    printf( "dissecting %dx%d grid based at %d:\n", width, height, g0->base ); 
#endif
    if( width < height ) {
      mid = height / 2;

      // 1st grid
      g1.ran[1][1] = g0->ran[1][0] + mid-1;

      // 2nd grid
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



// Compute a good row permutation given a column permutation for the
// sparse QR decomposition algorithm.
void row_order( const grid2 *g, const int *col_perm, int *row_perm )
{
  int size = Size(*g);
  int x, y, i, j, e;

  int *q = inverse_perm( col_perm, size );
  int *p = row_perm;

  for( i=0, j=0; i < size; ++i ) {
    Node(*g,q[i],x,y);
    
    if( InGrid2(*g,x-1,y) ) {
      e = HEIndex(*g,x-1,y);
      p[e] = j++;
    }

    if( InGrid2(*g,x,y-1) ) {
      e = VEIndex(*g,x,y-1);
      p[e] = j++;
    }
  }
  
  free_perm(q);
}


// Given row and column permutations compute the QR decomposition of a
// sparse matrix.
void sparse_qr_decomp( const int *P1,
		       sparse_mat *A,
		       const int *P2,
		       sparse_mat **Q,
		       sparse_mat **R )
{
  // Components of the R matrix:
  int nnz, *is, *js;
  double *vs;
  
  // Allocate structures to store Q and R. 
  sparse_vec **rows = (sparse_vec**)calloc( A->m, sizeof(sparse_vec*) );
  rotation   *grots = alloc_rotation( A->m );
  
  // Work variables
  int i, j, *col;
  double a, b, c, s, r, *val;
  sparse_vec row, *row_i, *row_j;

  grots -> n = A -> m;

  permute_rows(P1, A);
  permute_cols(A, P2);

  for( j = 0; j < A->m; ++j ) {
    row(*A,j,row);
    row_j = rows[j] = copy_sparse_vec( &row );

    if( row_j->nnz > 0 ) {
      i = *(row_j->is);
      b = *(row_j->vs);
	
       while( i < j ) {
	row_i = rows[i];
	
	if( row_i->nnz == 0 || *(row_i->is) > i )
	  a = 0.0;
	else
	  a = *(row_i->vs);
	
	cblas_drotg( &a, &b, &c, &s );
	sp_apply_grot( c, s, rows+i, rows+j );
	left_comp_rotation( &grots, i, j, c, s );

	row_j = rows[j];
	if( row_j->nnz == 0 ) break;

	i = *(row_j->is);
	b = *(row_j->vs);
      }
    }
  }

  *Q = sparse_mat_rotation( grots );
  *R = sparse_mat_rows( A->n, rows );

  free_rotation( grots );

  for( j = 0; j < A->m; ++j )
    free( rows[j] );
  free( rows );
}


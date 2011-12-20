#include <stdio.h>

#include "sparse_mx.h"
#include "macros.h"

mxArray *perm_to_mxarray( const int *P, int n )
{
  register const int *p = P;
  
  mxArray *A = mxCreateSparse( n, n, n, mxREAL );
  register mwIndex *i = mxGetIr( A );
  register mwIndex *j = mxGetJc( A );
  register double  *v = mxGetPr( A );

  const mwIndex *last_i = i + n;

  for( *(j++) = 0; i < last_i; ++p, ++i, ++j, ++v ) {
    *i = *p;
    *j = *(j-1) + 1;
    *v = 1.0;
  }

  return A;
}

void check_col( int col, int *cols, int nnz, int num )
{
  int count = 0;
  int *c;
  for( c = cols; c < cols+nnz; ++c )
    if( *c == col )
      count++;

  if( count > num )
    printf( "Column %d (%d entries): has %d more entries than counted.\n", 
	    col, num, count-num );
  else if( count < num )
    printf( "Column %d (%d entries): has %d less entries than counted.\n", 
	    col, num, num-count );
  /*
  else
    printf( "Column %d (%d entries): okay.\n", col, num );
  */
}

mxArray *sparse_to_mxarray( const sparse_mat *A )
{
  register const int     *i = A->is;
  register const int     *j = A->js;
  register const double  *v = A->vs;
  
  mxArray *mxA = mxCreateSparse( A->m, A->n, A->nnz, mxREAL );
  register mwIndex *mxi = mxGetIr( mxA );
  register mwIndex *mxj = mxGetJc( mxA );
  register double  *mxv = mxGetPr( mxA );
  
  if( A->row_major ) {
    register int row;
    register int col;
    register int idx;
    
    *(mxj++) = 0;
    for(col = 0; col < A->n; ++col, ++mxj ) {
      *mxj = *(mxj-1);
      for( i = A->is, row = 0; row < A->m; ++i, ++row ) {
	binsearch( col, j, *i, *(i+1), idx );
	if( idx >= 0 ) {
	  *(mxi++) = row;
	  *(mxv++) = v[idx];
	  (*mxj)++;
	}
      }

      check_col( col, j, A->nnz, *mxj-*(mxj-1) );
    }
  } else {    
    const int *last_i = A->is + A->nnz;
    const int *last_j = A->js + A->n + 1;
    const double *last_v = A->vs + A->nnz;

    copy( i, last_i, mxi );
    copy( j, last_j, mxj );
    copy( v, last_v, mxv );
  }

  return mxA;
}

sparse_mat *mxarray_to_sparse( const mxArray *mxA, unsigned row_major )
{
  sparse_mat *A;

  if( row_major ) {
    mwSize M = mxGetM( mxA );
    mwSize N = mxGetN( mxA );
    mwSize nnz = mxGetNzmax( mxA );

    register const mwIndex *mxi = mxGetIr( mxA );
    register const mwIndex *mxj = mxGetJc( mxA );
    register const double  *mxv = mxGetPr( mxA );
    
    register int     *i;
    register int     *j;
    register double  *v;
    register int     row;
    register int     col;
    register int     idx;
  
    A = alloc_sparse_mat( M, N, nnz, row_major );
    i = A->is;
    j = A->js;
    v = A->vs;
    
    *(i++) = 0;
    for(row = 0; row < M; ++row, ++i ) {
      *i = *(i-1);
      for( mxj = mxGetJc(mxA), col = 0; col < N; ++mxj, ++col ) {
	binsearch( row, mxi, *(mxj), *(mxj+1), idx );
	if( idx >= 0 ) {
	  *(j++) = col;
	  *(v++) = mxv[idx];
	  (*i)++;
	}
      }
    }
  } else
    A = init_sparse_mat( mxGetM(mxA),  mxGetN(mxA), mxGetNzmax(mxA), 0,
			 mxGetIr(mxA), mxGetJc(mxA), mxGetPr(mxA) );

  return A;
}

			  

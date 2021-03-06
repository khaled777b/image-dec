/*
 * MATLAB mex-file that implements the function spqr.
 *
 *   [Q R P1 P2] = spqr_grid2(A,sz);
 *
 * Computes the QR-decomposition of the sparse matrix A, which is
 * defined by:
 *                _ _
 *               | R |
 *   Q*P1*A*P2 = | - |
 *               |_0_|
 *
 * The matrix A is assumed to be an operator on functions over
 * 2-dimensional grids of size sz. Therefore, sz must be a vector of
 * length 2. 
 * 
 */

#include <grid2.h>
#include <sparse.h>
#include <sparse_qr_decomp.h>

#include <mex.h>
#include <matrix.h>
#include "sparse_mx.h"

void mxSPQR( const mxArray *mxA,
	     unsigned width, unsigned height,
	     mxArray **mxQ,  mxArray **mxR, 
	     mxArray **mxP1, mxArray **mxP2 );

void mexFunction( int nlhs, mxArray *plhs[],
		  int nrhs, const mxArray *prhs[] )
{
  double *sz;

  if(nrhs != 2)
    mexErrMsgTxt("Wrong # of inputs! spqr requires 2 sparse matrix inputs.");
  else if(nlhs != 4)
    mexErrMsgTxt("Wrong # of outputs! spqr produces 4 sparse matrix outputs.");

  if( mxIsComplex(prhs[0]) )
    mexErrMsgTxt("Complex input is not supported.");

  if( !mxIsSparse(prhs[0]) ) 
    mexErrMsgTxt("First input must be a sparse matrix.");

  if( mxGetNumberOfElements(prhs[1]) != 2 ) 
    mexErrMsgTxt("Second input must be a 2d vector with the grid size.");

  sz = mxGetPr(prhs[1]);

  mxSPQR( prhs[0], sz[0], sz[1], plhs, plhs+1, plhs+2, plhs+3 );
}

void mxSPQR( const mxArray *mxA,
	     unsigned width, unsigned height,
	     mxArray **mxQ,  mxArray **mxR, 
	     mxArray **mxP1, mxArray **mxP2 )
{
  sparse_mat *A = mxarray_to_sparse(mxA,1);
  sparse_mat *Q, *R;
  
  grid2 g = { 0, {{0,width},{0,height}} };
  unsigned M = NumEdges(g);
  unsigned N = Size(g);  
  int *P1 = identity_perm( M );
  int *P2 = identity_perm( N );
  
  grid_dissection_order( &g, P2 );
  row_order( &g, P2, P1 );

  sparse_qr_decomp( P1, A, P2, &Q, &R );

  *mxQ  = sparse_to_mxarray( Q );
  *mxR  = sparse_to_mxarray( R );
  *mxP1 = perm_to_mxarray( P1, M );
  *mxP2 = perm_to_mxarray( P2, N );

  free_perm( P1 );
  free_perm( P2 );
  free_sparse( Q );
  free_sparse( R );
  free_sparse( A );
}

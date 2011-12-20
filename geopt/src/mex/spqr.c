/*
 * MATLAB mex-file that implements the function spqr.
 *
 *   [Q R P1 P2] = spqr(A);
 *
 * Computes the QR-decomposition of the sparse matrix A, which is
 * defined by:
 *
 *                _ _
 *               | R |
 *   Q*P1*A*P2 = | - |
 *               |_0_|
 *
 * 
 */

#include <mex.h>

void mexFunction( int nlhs, mxArray *plhs[],
		  int nrhs, const mxArray *prhs[] )
{
  if(nrhs != 1)
    mexErrMsgTxt("Wrong # of inputs! spqr requires one sparse matrix input.");
  else if(nlhs != 4)
    mexErrMsgTxt("Wrong # of outputs! spqr produces 4 sparse matrix outputs.");

  if( !mxIsSparse(prhs[0]) ) 
    mexErrMsgTxt("Input must be a sparse matrix.");

  if( mxIsComplex(prhs[0]) ) 
    mexErrMsgTxt("Complex input is not supported.");

  mxSPQR( prhs[0], plhs, plhs+1, plhs+2, plhs+3 );
}

void mxSPQR( const mxArray *mxA, 
	     mxArray **mxQ,  mxArray **mxR, 
	     mxArray **mxP1, mxArray **mxP2 )
{
  sparse_mat *A = 
}

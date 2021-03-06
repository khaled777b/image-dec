
#include <stdio.h>
#include <mat.h>
#include <cblas.h>

#include "grid2.h"
#include "sparse.h"
#include "sparse_qr_decomp.h"
#include "mex/sparse_mx.h"

int main(int argc, char *argv[]) {

  MATFile  *mfp = matOpen(argv[1],"u");
  mxArray  *mxD = matGetVariable(mfp,"D");
  mxArray  *mxDims = matGetVariable(mfp,"dims");
  double   *d = mxGetPr( mxDims );
  unsigned  dims[2] = {d[0], d[1]};

  sparse_mat *D = mxarray_to_sparse(mxD,true);

  grid2 g = { 0, {{0,dims[0]},{0,dims[1]}} };  
  int *col_perm = identity_perm( Size(g) );
  int *row_perm = identity_perm( NumEdges(g) );
  sparse_mat *Q, *R;

  mxArray *P1;
  mxArray *P2;
  mxArray *mxQ;
  mxArray *mxR;

  grid_dissection_order( &g, col_perm );
  row_order( &g, col_perm, row_perm );

  sparse_qr_decomp( row_perm, D, col_perm, &Q, &R );

  P1  = perm_to_mxarray(row_perm, NumEdges(g));
  P2  = perm_to_mxarray(col_perm, Size(g));

  printf( "***************** Converting Q to mxArray *****************\n" ); 
  mxQ = sparse_to_mxarray(Q);

  printf( "***************** Converting R to mxArray *****************\n" ); 
  mxR = sparse_to_mxarray(R);

  free( col_perm );
  free( row_perm );
  free( Q );
  free( R );

  matPutVariable(mfp, "Q", mxQ);
  matPutVariable(mfp, "R", mxR);
  matPutVariable(mfp, "P1", P1);
  matPutVariable(mfp, "P2", P2);

  mxDestroyArray( P1  );
  mxDestroyArray( P2  );
  mxDestroyArray( mxQ );
  mxDestroyArray( mxR );

  return 0;
}

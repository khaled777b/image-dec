
#include <iostream>
#include <mat.h>
#include <cblas.h>

extern "C" {
#include "grid2.h"
#include "sparse.h"
#include "sparse_qr_decomp.h"
#include "mex/sparse_mx.h"
}

int main(int argc, char *argv) {

  MATFile  *mfp = matOpen(argv[1],"u");
  mxArray  *mxD = matGetVariable(mfp,"D");
  mxArray  *mxDims = matGetVariable(mfp,"dims");
  double   *d = mxGetPr( mxDims );
  unsigned  dims[2] = {d[0], d[1]};

  sparse_mat *D = mxarray_to_sparse(mxD,true);
  
  cout << "Constructing "<<dims[0]<<"x"<<dims[1]<<" grid.\n";
  grid2 g = { 0, {{0,dims[0]},{0,dims[1]}} };

  cout << "Computing column permutation.\n";
  int *col_perm = identity_perm( Size(g) );
  grid_dissection_order( &g, col_perm );

  cout << "Computing row permutation.\n";
  int *row_perm = identity_perm( NumEdges(g) );
  row_order( &g, col_perm, row_perm );

  sparse_mat *Q, *R;
  sparse_qr_decomp( row_perm, D, col_perm, &Q, &R );

  mxArray *P1  = perm_to_mxarray(row_perm, NumEdges(g));
  mxArray *P2  = perm_to_mxarray(col_perm, Size(g));
  mxArray *mxQ = sparse_to_mxarray(Q);
  mxArray *mxR = sparse_to_mxarray(R);

  free( col_perm );
  free( row_perm );
  free( mxQ );
  free( mxR );

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

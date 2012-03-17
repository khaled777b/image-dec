#ifndef SPARSE_QR_DECOMP_H
#define SPARSE_QR_DECOMP_H

#include "grid2.h"
#include "perm.h"
#include "givens.h"
#include "sparse.h"

void grid_dissection_order( const grid2 *g0, int *perm );
void row_order( const grid2 *g, const int *col_perm, int *row_perm );

void sparse_qr_decomp( const int *P1,
		       sparse_mat *A,
		       const int *P2,
		       sparse_mat **Q,
		       sparse_mat **R );

#endif

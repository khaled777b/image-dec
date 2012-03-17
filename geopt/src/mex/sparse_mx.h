#include <matrix.h>
#include <sparse.h>

mxArray *perm_to_mxarray( const int *P, int n );
mxArray *sparse_to_mxarray( const sparse_mat *A );
sparse_mat *mxarray_to_sparse( const mxArray *mxA, unsigned row_major );

			  

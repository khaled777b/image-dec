#include <iostream>
#include <iomanip>
#include <cblas.h>

extern "C" {
#include "grid2.h"
#include "sparse.h"
#include "sparse_qr_decomp.h"
}

using namespace std;

#define TEST_SPARSE 0

int main() {
  grid2 g = { 0, {{0,4},{0,6}} };

  cout << "Test Grid:\n"
       << "  Width  = " << Width(g)  << endl
       << "  Height = " << Height(g) << endl
       << "  Size   = " << Size(g)   << endl;
//        << "  Node(7,8): Index = " << Index(g,7,8) << endl
//        << "  Edge[(7,8) -> (8,8)]: Index = " << HEIndex(g,7,8) << endl
//        << "  Edge[(7,8) -> (7,9)]: Index = " << VEIndex(g,7,8) << endl;
  cout << endl;

#if TEST_SPARSE  
  int i, j;
  int dim = 15;
  int nnz1 = 4;
  int is1[] = {3, 6, 7, 10};
  double vs1[] = {0.5, -1.0, 2.0, -0.3};
  sparse_vec *v1 = init_sparse_vec( dim, nnz1, is1, vs1 );
                   
  cout << "Created a sparse vector v_1 \\in R^"<<dim<<endl;
  for( i=0; i<v1->nnz; ++i )
    cout << "\tv("<<v1->is[i]<<") = "<< v1->vs[i]<<"; "; 
  cout << endl;
  cout << endl;

  int nnz2 = 4;
  int is2[] = {3, 8, 11, 13};
  double vs2[] = {1.3, -0.7, -3.2, 1.0};
  sparse_vec *v2 = init_sparse_vec( dim, nnz2, is2, vs2 );

  cout << "Created a sparse vector v_2 \\in R^"<<dim<<endl;
  for( i=0; i<v2->nnz; ++i )
    cout << "\tv("<<v2->is[i]<<") = "<< v2->vs[i]<<"; "; 
  cout << endl;
  cout << endl;

  double c, s;
  cblas_drotg( vs1, vs2, &c, &s );

  sp_apply_grot( c, s, &v1, &v2 );

  for( i=0; i<v1->nnz; ++i )
    cout << "v1: \tv("<<v1->is[i]<<") = "<< v1->vs[i]<<"; "; 
  cout << endl;

  for( i=0; i<v2->nnz; ++i )
    cout << "v2: \tv("<<v2->is[i]<<") = "<< v2->vs[i]<<"; "; 
  cout << endl;
  cout << endl;

  
  int is[] = {0,2,4,6,8,10};
  int js[] = {0,1, 1,2, 2,3, 0,2, 1,3};
  double vs[] = {-1,1, -1,1, -1,1, -1,1, -1,1};
  sparse_mat *A = init_sparse_mat( 5, 4, 10, true, is, js, vs );

  int *col=A->js;
  double *v = A->vs;
  for( i=0; i<A->m; ++i ) {
    for( j=0; j<A->n; ++j )
      if( *col == j ) {
	cout << setw(4) << *(v++);
	col++;
      } else
	cout << setw(4) << 0.0;
    cout << endl;
  }
  cout << endl;

  int p1[] = {2,3,0,4,1};
  int p2[] = {1,3,2,0};

  permute_rows( p1, A );

  col=A->js;
  v = A->vs;
  for( i=0; i<A->m; ++i ) {
    for( j=0; j<A->n; ++j )
      if( *col == j ) {
	cout << setw(4) << *(v++);
	col++;
      } else
	cout << setw(4) << 0.0;
    cout << endl;
  }
  cout << endl;

  permute_cols( A, p2 );

  col=A->js;
  v = A->vs;
  for( i=0; i<A->m; ++i ) {
    for( j=0; j<A->n; ++j )
      if( *col == j ) {
	cout << setw(4) << *(v++);
	col++;
      } else
	cout << setw(4) << 0.0;
    cout << endl;
  }
  cout << endl;

  free( v1 );
  free( v2 );
  free( A  );
#endif

  cout << "Computing column permutation...\n";
  int *col_perm = identity_perm( Size(g) );
  grid_dissection_order( &g, col_perm );

  int k;
  for( k=0; k<Size(g); ++k )
    cout << k << " --> " << col_perm[k] << endl;

  cout << "Computing row permutation...\n";
  int *row_perm = identity_perm( NumEdges(g) );
  row_order( &g, col_perm, row_perm );

  for( k=0; k<NumEdges(g); ++k )
    cout << k << " --> " << row_perm[k] << endl;
  
  int is[] = { 0,  2,  4,  6,  8, 
	       10, 12, 14, 16, 18,
	       20, 22, 24, 26, 28,
	       30, 32, 34, 36, 38,
	       40, 42, 44, 
	       46, 48, 50,
	       52, 54, 56,
	       58, 60, 62,
	       64, 66, 68,
	       70, 72, 74, 76 };

  int js[] = { 0,1, 1,2, 2,3, 3,4, 4,5,
	       6,7, 7,8, 8,9, 9,10, 10,11,
	       12,13, 13,14, 14,15, 15,16, 16,17,
	       18,19, 19,20, 20,21, 21,22, 22,23,
	       0,6, 1,7, 2,8,
	       3,9, 4,10, 5,11,
	       6,12, 7,13, 8,14,
	       9,15, 10,16, 11,17,
	       12,18, 13,19, 14,20,
	       15,21, 16,22, 17,23 };

  double vs[] = { -1,1, -1,1, -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1,
		  -1,1, -1,1, -1,1 };

  sparse_mat *A = init_sparse_mat( 38, 24, 76, true, is, js, vs );
  sparse_mat *Q, *R;

  sparse_qr_decomp( row_perm, A, col_perm, &Q, &R );

  free( col_perm );
  free( row_perm );
  free( A );
  free( Q );
  free( R );
  
  return 0;
}

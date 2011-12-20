#ifndef SPARSE_H
#define SPARSE_H

#include <blas.h>
#include <string.h>

typedef struct sp_vec {
  int n;
  int nnz;
  int *is;

  double *vs;
} sparse_vec;

typedef struct sp_mat {
  unsigned row_major;

  int m;
  int n;
  int nnz;
  int *is;
  int *js;

  double *vs;
} sparse_mat;


sparse_vec *alloc_sparse_vec( int n, int nnz );
sparse_mat *alloc_sparse_mat( int m, int n, int nnz, unsigned row_maj );
sparse_vec *init_sparse_vec( int n, int nnz, const int *is, const double *vs );
sparse_mat *init_sparse_mat( int m, int n, int nnz, unsigned row_maj, 
			     const int *is, const int *js, const double *vs );

sparse_mat *sparse_mat_rows( int m, sparse_vec **rows );

void sp_apply_grot( double c, double s, sparse_vec **u, sparse_vec **v );
void permute_rows( const int *perm, sparse_mat *A );
void permute_cols( sparse_mat *A, const int *perm );


/* MACROS:
   ----------

   row(A,i,row)
   col(A,i,col)
   sparse_to_blas(A,B)
*/

#define copy_sparse_vec(v) init_sparse_vec( (v)->n, (v)->nnz, (v)->is, (v)->vs )

#define free_sparse( a ) free(a);

#define row(A,i,row)					\
    if( (A).row_major ) {				\
      (row).n = (A).n;					\
      (row).nnz = (A).is[i+1] - (A).is[i];		\
      (row).is = (A).js + (A).is[i];			\
      (row).vs = (A).vs + (A).is[i];			\
    }

#define col(A,i,col)					\
    if( !(A).row_major ) {				\
      (col).n = (A).m;					\
      (col).nnz = (A).js[i+1] - (A).js[i];		\
      (col).is = (A).is + (A).js[i];			\
      (col).vs = (A).vs + (A).js[i];			\
    }


#define rowmaj_to_blas(A,B) {					\
    int r;							\
    B = BLAS_duscr_begin( (A).m, (A).n );			\
    BLAS_ussp( B, blas_rowmajor );				\
								\
    for( r=0; r<(A).m; ++r )					\
      BLAS_duscr_insert_row( B, r, (A).is[r+1]-(A).is[r],	\
			     (A).vs[r], (A).js[r] );		\
    BLAS_duscr_end( B );					\
  }

#define colmaj_to_blas(A,B) {					\
    int c;							\
    B = BLAS_duscr_begin( (A).m, (A).n );			\
    BLAS_ussp( B, blas_colmajor );				\
								\
    for( c=0; c<(A).n; ++c )					\
      BLAS_duscr_insert_col( B, c, (A).is[c+1]-(A).is[c],	\
			     (A).vs[c], (A).js[c] );		\
    BLAS_duscr_end( B );					\
  }

#define sparse_to_blas(A,B)   \
    if((A).row_major)	      \
      rowmaj_to_blas(A,B)     \
    else		      \
      colmaj_to_blas(A,B)

#endif

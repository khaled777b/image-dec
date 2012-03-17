#include <cblas.h>
#include <stdlib.h>

#include "sparse.h"
#include "perm.h"

sparse_vec *alloc_sparse_vec( int n, int nnz )
{
  int hdr  = sizeof(sparse_vec);
  int idxs = nnz*sizeof(int);
  int vals = nnz*sizeof(double);

  void *mem = malloc( hdr+idxs+vals );
  sparse_vec *v = (sparse_vec*)mem;
  v->n = n;
  v->nnz = nnz;

  v->is = mem + hdr;
  v->vs = mem + hdr + idxs;

  return v;
}

sparse_mat *alloc_sparse_mat( int m, int n, int nnz, unsigned row_maj )
{
  int hdr = sizeof(sparse_mat);
  int maj = row_maj? (m+1)*sizeof(int) : (n+1)*sizeof(int);
  int min = nnz*sizeof(int);
  int dat = nnz*sizeof(double);

  void *mem = malloc( hdr+maj+min+dat );
  sparse_mat *A = (sparse_mat*)mem;

  A->row_major = row_maj;
  
  A->m = m;
  A->n = n;
  A->nnz = nnz;

  A->is = row_maj? mem + hdr : mem + hdr + maj;
  A->js = row_maj? mem + hdr + maj : mem + hdr;
  A->vs = mem + hdr + maj + min;

  return A;
}

sparse_vec *init_sparse_vec( int n, int nnz, const int *is, const double *vs )
{
  sparse_vec *v = alloc_sparse_vec( n, nnz );

  memcpy( v->is, is, nnz*sizeof(int) );
  memcpy( v->vs, vs, nnz*sizeof(double) );
  
  return v;
}

sparse_mat *init_sparse_mat( int m, int n, int nnz, unsigned row_maj,
			     const int *is, const int *js, const double *vs )
{
  sparse_mat *A = alloc_sparse_mat( m, n, nnz, row_maj );

  if( row_maj ) {
    memcpy( A->is, is, (m+1)*sizeof(int) );
    memcpy( A->js, js, nnz*sizeof(int) );
  } else {
    memcpy( A->is, is, nnz*sizeof(int) );
    memcpy( A->js, js, (n+1)*sizeof(int) );
  }

  memcpy( A->vs, vs, nnz*sizeof(double) );
  
  return A;
}

sparse_mat *sparse_mat_rows( int m, sparse_vec **rows )
{
  
  int k, nnz, n=(*rows)->n, row_nnz;
  int *is, *js, *j;
  double *vs, *v;
  sparse_mat *A;

  nnz = 0;
  is  = (int*)calloc( m+1, sizeof(int) );
  *is = 0;
  for( k = 0; k < m; ++k ) {
    nnz += rows[k]->nnz;
    is[k+1] = nnz;
  }

  A = alloc_sparse_mat( m, n, nnz, 1 );
  memcpy( A->is, is, (m+1)*sizeof(int) );
  free( is );

  j = A->js;
  v = A->vs;
  for( k = 0; k < m; ++k ) {
    row_nnz = rows[k]->nnz;
    memcpy( j, rows[k]->is, row_nnz*sizeof(int) );
    memcpy( v, rows[k]->vs, row_nnz*sizeof(double) );
    j += row_nnz;
    v += row_nnz;
  }  

  return A;
}


enum {
  FIRST   = 0x01,
  SECOND  = 0x02,
  BOTH    = 0x03,
};

void merge_find( int nx, int *x, 
		 int ny, int *y, 
		 int *n, int **xy, int **find ) 
{
  int cnt = 0;
  int *xi = x,    *yi = y;
  int *xn = x+nx, *yn = y+ny;

  int *z = (int*)calloc( nx+ny, sizeof(int) );
  int *f = (int*)calloc( nx+ny, sizeof(int) );

  int *zi = z, *fi = f;

  while( xi < xn &&  yi < yn ) {
    if( *xi == *yi ) {
      *(zi++) = *(xi++); ++yi;
      *(fi++) = BOTH;
    } else if( *xi < *yi ) {
      *(zi++) = *(xi++);
      *(fi++) = FIRST;
    } else { // *yi < *xi
      *(zi++) = *(yi++);
      *(fi++) = SECOND;
    }

    ++cnt;
  }

  while( xi < xn ) {
    *(zi++) = *(xi++);
    *(fi++) = FIRST;

    ++cnt;
  }

  while( yi < yn ) {
    *(zi++) = *(yi++);
    *(fi++) = SECOND;

    ++cnt;
  }

  *n  = cnt;
  *xy = realloc(z,cnt*sizeof(int));
  *find = realloc(f,cnt*sizeof(int));
}

static double zero = 1.11e-16;

void sp_apply_grot( double c, 
		    double s, 
		    sparse_vec **u,
		    sparse_vec **v )		    
{
  int i, nnz, nnz_u, nnz_v;
  int *is, *find, *iu, *iv;
  double *us, *vs;
  double *ui=(*u)->vs, *vi=(*v)->vs;
  sparse_vec *uu, *vv;

  merge_find( (*u)->nnz, (*u)->is, (*v)->nnz, (*v)->is, &nnz, &is, &find );

  us = (double*)calloc(nnz,sizeof(double));
  vs = (double*)calloc(nnz,sizeof(double));
  
  for( i=0; i<nnz; ++i ) {
    us[i] =  find[i]&FIRST ? *(ui++) : 0.0;
    vs[i] =  find[i]&SECOND? *(vi++) : 0.0;      
  }

  free( find );

  cblas_drot( nnz, us, 1, vs, 1, c, s );
  
  nnz_u = nnz_v = nnz;
  for( i=0; i<nnz; ++i ) {
    if( fabs(us[i]) <= zero ) --nnz_u;
    if( fabs(vs[i]) <= zero ) --nnz_v;
  }

  uu = alloc_sparse_vec( (*u)->n, nnz_u );
  vv = alloc_sparse_vec( (*v)->n, nnz_v );

  iu = uu->is; iv = vv->is;
  ui = uu->vs; vi = vv->vs;
  for( i=0; i<nnz; ++i ) {
    if( fabs(us[i]) > zero ) {
      *(iu++) = is[i];
      *(ui++) = us[i];
    }

    if( fabs(vs[i]) > zero ) {
      *(iv++) = is[i];
      *(vi++) = vs[i];
    }
  }

  free( is );
  free( us );
  free( vs );
  free( *u );
  free( *v );

  *u = uu;
  *v = vv;
}


void minor_sort_merge( int n1, int *x1, double *y1,
		       int n2, int *x2, double *y2 )
{  
  int *x1n = x1 + n1;
  int *x2n = x2 + n2;

  int    *x = (int*)   calloc( n1+n2, sizeof(int)    );
  double *y = (double*)calloc( n1+n2, sizeof(double) );

  int    *xi = x, *x1i = x1, *x2i = x2;
  double *yi = y, *y1i = y1, *y2i = y2;
  
  while( x1i < x1n && x2i < x2n ) {
    if( *x2i < *x1i ) {
      *(xi++) = *(x2i++);
      *(yi++) = *(y2i++);
    } else {
      *(xi++) = *(x1i++);
      *(yi++) = *(y1i++);
    }
  }

  memcpy( xi, x1i, (x1n-x1i)*sizeof(int)    );
  memcpy( yi, y1i, (x1n-x1i)*sizeof(double) );

  memcpy( xi, x2i, (x2n-x2i)*sizeof(int)    );
  memcpy( yi, y2i, (x2n-x2i)*sizeof(double) );

  memcpy( x1, x, (n1+n2)*sizeof(int)    );
  memcpy( y1, y, (n1+n2)*sizeof(double) );

  free( x );
  free( y );
}

void minor_sort( int n, int *x1, double *y1 ) {
  if( n > 1 ) {
    int n1 = n / 2;
    int n2 = n - n1;
    
    int    *x2 = x1 + n1;
    double *y2 = y1 + n1;
    
    minor_sort( n1, x1, y1 );
    minor_sort( n2, x2, y2 );
    
    minor_sort_merge( n1, x1, y1, n2, x2, y2 );
  }
}

void sort_minor_rows( sparse_mat *A ) {
  int *jn = A->js + A->n;

  int  n;
  int *i = A->is, *j = A->js;
  double *v = A->vs;  

  while( j < jn ) {
    n = *(j+1) - *j;
    minor_sort( n, i, v );
    j++;
    i += n;
    v += n;
  }
}

void sort_minor_cols( sparse_mat *A ) {
  int *in = A->is + A->m;

  int  n;
  int *i = A->is, *j = A->js;
  double *v = A->vs;  

  while( i < in ) {
    n = *(i+1) - *i;
    minor_sort( n, j, v );
    i++;
    j += n;
    v += n;
  }
}

void permute_rows( const int *perm, sparse_mat *A ) {
  if( A->row_major ) {
    double *vs = (double*)calloc(A->nnz,sizeof(double));
    int    *is = (int*)calloc(A->m+1,sizeof(int));
    int    *js = (int*)calloc(A->nnz,sizeof(int));
    int    *q  = inverse_perm( perm, A->m );
    
    int    *qi   = q;
    int    *dstj = js;
    double *dst  = vs;
    double *src;
    int    *srcj, *row;
    int     i, k, n;

    is[0] = 0;
    for( i=0, k=0; i < A->m; ++i, ++qi ) {
      row  = A->is + *qi;
      srcj = A->js + *row;
      src  = A->vs + *row;      

      n = *(row+1) - *row;
      memcpy( dstj, srcj, n*sizeof(int) );
      memcpy( dst,  src,  n*sizeof(double) );

      k    += n;
      dstj += n;
      dst  += n;

      is[i+1] = k;
    }

    memcpy( A->is, is, (A->m+1)*sizeof(int) );
    memcpy( A->js, js, A->nnz*sizeof(int) );
    memcpy( A->vs, vs, A->nnz*sizeof(double) );

    free( q );       
    free( is );
    free( js );
    free( vs );    
  } else {
    int k, *ik=A->is;
    for( k=0; k < A->nnz; ++k, ++ik )
      *ik = perm[*ik];

    sort_minor_rows(A);
  }
}

void permute_cols( sparse_mat *A, const int *perm ) {
  if( A->row_major ) {
    int k, *jk=A->js;
    for( k=0; k < A->nnz; ++k, ++jk )
      *jk = perm[*jk];
    
    sort_minor_cols(A);
  } else {
    double *vs = (double*)calloc(A->nnz,sizeof(double));
    int    *is = (int*)calloc(A->nnz,sizeof(int));
    int    *js = (int*)calloc(A->n,sizeof(int));
    int    *q  = inverse_perm( perm, A->n );
    
    int    *qj   = q;
    int    *dsti = is;
    double *dst  = vs;
    double *src;
    int    *srci, *col;
    int     j, k, n;

    js[0] = 0;
    for( j=0, k=0; j < A->n; ++j, ++qj ) {
      col  = A->js + *qj;
      srci = A->is + *col;
      src  = A->vs + *col;      

      n = *(col+1) - *col;
      memcpy( dsti, srci, n*sizeof(int) );
      memcpy( dst,  src,  n*sizeof(double) );

      k    += n;
      dsti += n;
      dst  += n;

      js[j+1] = k;
    }

    memcpy( A->is, is, (A->m+1)*sizeof(int) );
    memcpy( A->js, js, A->nnz*sizeof(int) );
    memcpy( A->vs, vs, A->nnz*sizeof(double) );

    free( q );       
    free( is );
    free( js );
    free( vs );    
  }
}

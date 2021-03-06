#ifndef GIVENS_H
#define GIVENS_H

#include "sparse.h"

typedef struct {
  int i;
  int j;
  double c;
  double s;
} rot;

typedef struct {
  int  n;
  int  nrots;
  int  capacity;
  rot *rots;
} rotation;

static rotation *alloc_rotation( int cap ) {
  int hdr = sizeof(rotation); 
  int dat = cap*sizeof(rot);

  char *mem = (char*)malloc(hdr+dat);
  rotation *r = (rotation*)mem;
  
  r->nrots = 0;
  r->capacity = cap;
  r->rots = (rot*)(mem+hdr);

  return r;
}

static rotation *realloc_rotation( rotation *r, int newcap ) {
  int hdr = sizeof(rotation); 
  int dat = newcap*sizeof(rot);

  char *mem = (char*)realloc((void*)r,hdr+dat);
  rotation *rr = (rotation*)mem;
  
  rr->capacity = newcap;
  rr->rots = (rot*)(mem+hdr);

  return rr;
}

#define free_rotation(r) free(r)

static void left_comp_rotation( rotation **rs, int i, int j, double c, double s ) {
  rot *gr;
  rotation *r = *rs;
  
  if( r->nrots+1 >= r->capacity )
    r = *rs = realloc_rotation( r, 2*(r->capacity) );

  gr = r->rots + r->nrots;
  gr->i = i;
  gr->j = j;
  gr->c = c;
  gr->s = s;
  r->nrots++;
}

static sparse_mat *sparse_mat_rotation( const rotation *rs ) {
  const double one = 1;
  const rot *last_rot = rs->rots + rs->nrots;

  int k;
  rot *r;
  sparse_vec **rows = (sparse_vec**)calloc( rs->n, sizeof(sparse_vec*) );
  sparse_mat *R;

  /* Initialize rows of a spase identity matrix. */
  for( k = 0; k < rs->n; ++k )
    rows[k] = init_sparse_vec( rs->n, 1, &k, &one );

  /* Apply rotations to the rows. */
  for( r = rs->rots; r < last_rot; ++r )
    sp_apply_grot( r->c, r->s, rows+r->i, rows+r->j );

  /* Build sparse matrix from rows. */
  R = sparse_mat_rows( rs->n, rows );

  /* Free the row vectors. */
  for( k = 0; k < rs->n; ++k )
    free( rows[k] );
  free( rows );

  return R;
}

#endif


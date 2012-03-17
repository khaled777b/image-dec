#ifndef PERM_H
#define PERM_H

static int *identity_perm( int size ) {
  int i;
  int *p = (int*)calloc( size, sizeof(int) );
  for( i=0; i<size; ++i )
    p[i] = i;
  return p;
}
    
static int *inverse_perm( const int *perm, int size ) {
  int i;
  int *p = (int*)calloc( size, sizeof(int) );
  for( i=0; i<size; ++i )
    p[perm[i]] = i;
  return p;
}

#define free_perm( perm ) free(perm)

#endif


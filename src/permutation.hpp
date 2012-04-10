
#include <cstdlib>

#include <boost/numeric/mtl/mtl.hpp>

typedef mtl::dense_vector< size_t > perm_t;
typedef mtl::compressed2D< double > perm_mat;

inline perm_mat permutation_matrix( const perm_t& perm )
{
  return mtl::matrix::permutation< double >( perm );
}


inline perm_t inverse_perm( const perm_t& p )
{
  const size_t N = size(p);
  
  perm_t q( N );
  
  for( size_t i = 0; i < N; ++i )
    {
      q[ p[i] ] = i;
    }

  return q;
}

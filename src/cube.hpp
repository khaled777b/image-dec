
#include <boost/array.hpp>

#include <metamath.hpp>

template< size_t D, typename V = size_t >
struct cube :
  public boost::array< V, pow2<D>::value >
{
  typedef V               vertex_type;
  typedef cube< D-1, V >  facet_type;
  
  static const size_type dim = D;
  static const size_type num_verts = pow2<D>::value;

  typedef boost::array< index_type, dim > vertex_array;
  
  typedef typename vertex_array::value_type             value_type;
  typedef typename vertex_array::size_type              size_type;
  typedef typename vertex_array::difference_type        difference_type;
  typedef typename vertex_array::iterator               iterator;
  typedef typename vertex_array::const_iterator         const_iterator;
  typedef typename vertex_array::reverse_iterator       reverse_iterator;
  typedef typename vertex_array::const_reverse_iterator const_reverse_iterator;

  typedef boost::array< facet_type, times2<D>::value >  boundary_type;  

  cube( const vertex_type*  verts );
  cube( const vertex_array& verts );  
  cube( const facet& front, const facet& back );

  /// Reverses the orientation of the D-cube.
  void invert();
  
  /// Returns the boundary of the D-cube as an array of its facets,
  /// which are (D-1)-cubes.
  boundary_type boundary() const;
};

/// Computes the 'cylinder' of two D-cubes, which creates a
/// (D+1)-cube.
template< size_t D, typename V >
cube< D+1, V > operator+ ( const cube<D,V>& c1, const cube<D,V>& c2 )
{
}



#ifndef GRID_TOPOLOGY_HPP
#define GRID_TOPOLOGY_HPP

#include <boost/array.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/comparison.hpp>

namespace mpl = boost::mpl;

namespace grid
{  

  namespace detail
  {
    template< typename _Grid, size_t _Dim >
    class skeleton
    {
    public:
      typedef _Grid  grid_type;
      
      typedef grid_type::size_type   size_type;
      typedef grid_type::index_type  index_type;
      
      static const size_type grid_dim = grid_type::dim;
      static const size_type dim      = _Dim;
    private:      
      static const size_type aux_dims =
        mpl::if_c< dim > grid_dim / 2,
        /* then */ mpl::size_t< dim >,
        /* else */ mpl::size_t< grid_dim - dim >
        >::value;
      
    };
  }
  
  namespace layout 
  {
    template< size_t _Grid >
    struct row_major 
    {
      typedef _Grid grid_type;

      typedef typename grid_type::size_type  size_type;
      typedef typename grid_type::node_type  node_type;
      
      static const dim = grid_type::dim;

      row_major( const grid_type& grid );

      size_type offset( const node_type& node );
      
      node_type node( size_type offs );
      void node( size_type offs, node_type& crd );      
    private:
      const grid_type&  m_grid;
      const node_type   m_slice;
    };
  }

  template< size_t   _Dim,
            typename _Index  = size_t,
            template< typename > class _Layout = layout::row_major >
  class topology
  {
  public:
    typedef size_t  size_type;
    typedef _Index  index_type;

    typedef topology< _Dim, _Index, _Layout >  grid_topology;
    typedef _Layout< grid_topology >           grid_layout;

    static const Size dim = _Dim;

    template< size_type D >
    struct skeleton;

    typedef typename skeleton<0>::node_type node_type;
  protected:
    typename skeleton<0>::node_type m_size;
  };

  namespace layout
  {
    template< size_t G >
    inline row_major<G>::row_major( const grid_type& grid ) :
      m_grid( grid ) 
    {
      m_slice[ dim-1 ] = 1;
      for( size_type i = dim-1; i > 0; --i )
        m_slice[ i-1 ] = m_grid.size( i ) * m_slice[ i ];
    }

    template< size_t G >
    typename row_major<G>::size_type
    inline row_major<G>::offset( const coord_type& coord )
    {
      size_type offs = 0;
      
      for( size_type i = 0; i < dim; ++i )
        offs += coord[i] * m_slice[i];
      
      return offs;
    }

    coord_type coord( size_type offs )
    {
      coord_type crd;
      coord( offs, crd );
      return crd;
    }
    
    void coord( size_type offs, coord_type& crd )
    {
      for( size_type i = 0; i < dim; ++i )
        {
          crd[i]  = offs / m_slice[i];
          offs   -= crd[i] * m_slice[i];
        }
      
      return coord;
    }
}
  
}

#endif

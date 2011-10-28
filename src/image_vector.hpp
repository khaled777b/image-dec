/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_VECTOR_HPP
#define IMAGE_DEC_IMAGE_VECTOR_HPP

#include "vectors.hpp"
#include "image_types.hpp"

struct image_channel_to_vector
{
  typedef vector_t result_type;

  image_channel_to_vector( size_t chan )
    : m_chan( chan )
  {}

  template< class ImageView >
  result_type operator() ( const ImageView& view )
  {
    typedef typename boost::gil::nth_channel_view_type< ImageView >::type
      ChannelView;

    typedef typename ChannelView::iterator Iter;
    
    ChannelView chan_view = boost::gil::nth_channel_view( view, m_chan );

    size_t w = chan_view.width();
    size_t h = chan_view.height();
    
    vector_t v( w*h );
    
    size_t i = 0;
    for( Iter pix = chan_view.begin(); pix != chan_view.end(); ++pix )
      v[ i++ ] = static_cast< double >( (*pix)[0] );

    return v;
  }

  size_t m_chan;
};


vector_t nth_channel_vector( const image_t& image, size_t n )
{
  using namespace boost::gil;
  
  return apply_operation( const_view( image ), image_channel_to_vector( n ) );
}


#endif


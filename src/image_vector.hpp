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

struct vector_to_image_channel
{
  typedef void result_type;

  vector_to_image_channel( const vector_t& u, size_t chan )
    : m_chan( chan ),
      m_vec( u )
  {}

  template< class ImageView >
  result_type operator() ( ImageView& view )
  {
    typedef typename boost::gil::nth_channel_view_type< ImageView >::type
      ChannelView;

    typedef typename ChannelView::iterator Iter;
    
    ChannelView chan_view = boost::gil::nth_channel_view( view, m_chan );

    size_t w = chan_view.width();
    size_t h = chan_view.height();
    
    size_t i = 0;
    for( Iter pix = chan_view.begin(); pix != chan_view.end(); ++pix )
      (*pix)[0] = m_vec[ i++ ];
  }

  size_t    m_chan;
  vector_t  m_vec;
};


vector_t nth_channel_vector( const image_t& image, size_t n = 0 )
{
  using namespace boost::gil;
  
  return apply_operation( const_view( image ), image_channel_to_vector( n ) );
}


template< typename Image >
void nth_channel_from_vector( const vector_t& u, Image& image, size_t n = 0 )
{
  using namespace boost::gil;

  vector_to_image_channel vec_to_chan( u, n );
  return vec_to_chan( view( image ) );
  
  // return apply_operation( view( image ), vector_to_image_channel( u, n ) );
}


#endif


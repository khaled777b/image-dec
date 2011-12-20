/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_MTL_HPP
#define IMAGE_DEC_IMAGE_MTL_HPP

#include <boost/numeric/mtl/mtl.hpp>

#include "image_types.hpp"

struct image_channel_to_vector
{
  typedef mtl::dense_vector< double >  result_type;
  
};


#endif


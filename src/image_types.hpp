/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_TYPES_HPP
#define IMAGE_DEC_IMAGE_TYPES_HPP

#include <boost/mpl/vector.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>

using namespace boost::gil;

typedef boost::mpl::vector<
  boost::gil::gray8_image_t,
  boost::gil::rgb8_image_t,
  boost::gil::gray16_image_t,
  boost::gil::rgb16_image_t 
  >
image_types;

typedef boost::gil::any_image< image_types > image_t;

#endif


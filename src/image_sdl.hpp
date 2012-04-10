/*
 *  Copyright 2011 Kree Cole-McLaughlin
 */

#ifndef IMAGE_DEC_IMAGE_SDL_HPP
#define IMAGE_DEC_IMAGE_SDL_HPP

#include <cstdlib>
#include <iostream>

#include <SDL/SDL.h>

#include "image_types.hpp"

typedef SDL_Surface* Screen;
typedef SDL_Surface* SDLImage;

bool initializeSDL()
{
  static bool is_initialized = false;
  
  if( !is_initialized )
    {
      std::cout << " * Initializing SDL\n";
      
      if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
        {
          std::cout << "   ! Error: Could not initialize SDL.\n";
          return false;
        }
      
      atexit( SDL_Quit );

      is_initialized = true;
    }

  return is_initialized;
}

Screen openImageWindow( const image_t& image )
{
  size_t w = image.width();
  size_t h = image.height();
  
  Screen screen = SDL_SetVideoMode( w, h, 0, SDL_HWSURFACE );
  if( screen == NULL )
    {
      std::cout << " ! Error: Could not create SDL window.\n";
      return NULL;
    }

  SDL_WM_SetCaption( "ImageDEC", NULL );

  return screen;
}

Uint32 rmask, gmask, bmask, amask;

struct image_to_sdl
{
  typedef SDLImage result_type;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  static const Uint32 rmask = 0xff000000;
  static const Uint32 gmask = 0x00ff0000;
  static const Uint32 bmask = 0x0000ff00;
  static const Uint32 amask = 0x000000ff;
#else
  static const Uint32 rmask = 0x000000ff;
  static const Uint32 gmask = 0x0000ff00;
  static const Uint32 bmask = 0x00ff0000;
  static const Uint32 amask = 0xff000000;
#endif

  SDLImage createImage( const size_t w, const size_t h )
  {
    SDLImage image = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32,
                                           rmask, gmask, bmask, amask );

    if( image == NULL )
      {
        std::cout << " ! Error: Could not create SDL image.\n";
        return NULL;
      }
    
    return image;
  }

  template< class ImageView >
  SDLImage operator() ( const ImageView& orig_view )
  {
    using namespace boost::gil;
    
    typedef typename color_converted_view_type< ImageView, rgb8_pixel_t >::type
      RGBView;

    typedef typename RGBView::iterator Iter;

    RGBView  view  = color_converted_view< rgb8_pixel_t >( orig_view );
    SDLImage image = createImage( view.width(), view.height() );
    
    SDL_LockSurface( image );

    Uint32* pix = (Uint32*)( image->pixels );
    
    for( Iter it = view.begin(); it != view.end(); ++it, ++pix )
      {
        const rgb8_pixel_t rgb = *it;
        *pix = SDL_MapRGB( image->format, rgb[0], rgb[1], rgb[2] );
      }

    SDL_UnlockSurface( image );

    return image;
  }

};


void draw_image( const image_t& image, Screen screen )
{
  using namespace boost::gil;

  SDLImage sdl_image = apply_operation( const_view( image ), image_to_sdl() );

  SDL_Rect dest;
  dest.x = 0;
  dest.y = 0;
  dest.w = sdl_image->w;
  dest.h = sdl_image->h;
  
  SDL_BlitSurface( sdl_image, NULL, screen, &dest );
  SDL_UpdateRects( screen, 1, &dest );
  SDL_FreeSurface( sdl_image );
}

#endif


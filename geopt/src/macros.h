#ifndef __MACROS_H__
#define __MACROS_H__

#define copy( src, end, dst ) {		\
    for(; src < end; ++src, ++dst )	\
      *dst = *src;			\
  }


#define binsearch( x, v, lo, hi, idx ) {				\
    int _lo_ = lo, _hi_ = hi, _mid_;					\
									\
    idx = -1;								\
									\
    while( _lo_ < _hi_ ) {						\
      _mid_ = (_lo_ + _hi_) / 2;					\
      if( x == v[_mid_] )						\
	{ idx = _mid_; break; }						\
      else if( x < v[_mid_] )						\
	_hi_ = _mid_;							\
      else								\
	_lo_ = _mid_ + 1;						\
    }									\
  }


#endif

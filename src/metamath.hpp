
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/arithmetic.hpp>

namespace mpl = boost::mpl;

template< typename N >
struct pow2
{
  typedef typename N::tag        tag;
  typedef typename N::value_type value_type;

  typedef mpl::integral_c< value_type, 0 > zero;
  typedef mpl::integral_c< value_type, 1 > one;
  typedef mpl::integral_c< value_type, 2 > two;

  typedef typename mpl::if_<
    mpl::equal_to< N, zero >,
    one,
    mpl::times< two, pow2< typename N::prior > >
    >::type
  type;

  static const value_type value = type::value;
};

template< typename N >
struct times2
{
  typedef typename N::tag        tag;
  typedef typename N::value_type value_type;

  typedef mpl::integral_c< value_type, 0 > zero;
  typedef mpl::integral_c< value_type, 1 > one;
  typedef mpl::integral_c< value_type, 2 > two;

  typedef typename mpl::if_<
    mpl::equal_to< N, zero >,
    one,
    mpl::times< two, N >
    >::type
  type;

  static const value_type value = type::value;
};

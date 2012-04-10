#ifndef ARROWS_ARROW_HPP
#define ARROWS_ARROW_HPP

#include <boost/proto/proto.hpp>

#include <boost/mpl/logical.hpp>
#include <boost/mpl/comparison.hpp>

#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/include/join.hpp>

namespace mpl    = boost::mpl;
namespace proto  = boost::proto;
namespace fusion = boost::fusion;

#include "arrow_traits.hpp"

namespace arrows {  

  template< class E > struct arrow;

  struct arrow_grammar
    : proto::or_< proto::terminal< proto::_ >,
                  proto::shift_right< arrow_grammar, arrow_grammar >,
                  proto::comma< arrow_grammar, arrow_grammar >,
                  proto::logical_and< arrow_grammar, arrow_grammar >
                  >
  {};

  struct arrow_domain
    : proto::domain< proto::generator< arrow >, arrow_grammar >
  {};
  
  template< class E >
  struct arrow
    : proto::extends< E, arrow<E>, arrow_domain >
  {
    typedef arrow_traits< E > traits;

    typedef typename traits::input_type   input_type;
    typedef typename traits::output_type  output_type;

    explicit arrow( const E& e )
      : proto::extends< E, arrow<E>, arrow_domain >( e )
    {}
  };

  template< typename Expr >
  inline arrow< Expr > make_arrow( const Expr& expr )
  {
    return arrow< Expr >( expr );
  }

  template< typename F >
  inline arrow< typename proto::terminal< F >::type >
  arr( F fun )
  {
    typename proto::terminal< F >::type A = { fun };
    return make_arrow( A );
  }
 
}

#endif
  

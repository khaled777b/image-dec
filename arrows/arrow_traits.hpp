#ifndef ARROWS_ARROW_TRAITS_HPP
#define ARROWS_ARROW_TRAITS_HPP

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/function_types/is_function.hpp>
#include <boost/function_types/is_function_pointer.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/remove.hpp>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/join.hpp>

#include <boost/proto/proto.hpp>

namespace mpl    = boost::mpl;
namespace proto  = boost::proto;
namespace fusion = boost::fusion;

namespace arrows {  

  namespace 
  {
    template< typename T >
    struct function_input
    {
      typedef
      boost::function_types::parameter_types<T>
      param_types;
      
      typedef typename
      mpl::remove< param_types, void >::type
      non_void_params;

      typedef typename
      mpl::if_<
        mpl::empty< non_void_params >,
        fusion::vector0<>,
        typename fusion::result_of::as_vector< non_void_params >::type
        >::type
      type;
    };

    template< typename T >
    struct function_output
    {
      typedef typename
      boost::function_types::result_type<T>::type
      result_type;
      
      typedef typename
      mpl::if_<
        boost::is_same< result_type, void >,
        fusion::vector0<>,
        fusion::vector< result_type >
        >::type
      type;
    };
  }
  
  template< class A,
            class Tag = typename proto::tag_of< A >::type,
            class Enable = void >
  struct arrow_traits;

  template< class A >
  struct arrow_traits< A, proto::tag::terminal,
                       typename boost::enable_if< 
                         boost::is_function<
                           typename proto::result_of::value<A>::type
                           > >::type
                       >
  {
    typedef typename proto::result_of::value< A >::type function_type;

    typedef typename function_input< function_type >::type
    input_type;

    typedef typename function_output< function_type >::type
    output_type;
  };

  template< class A >
  struct arrow_traits< A, proto::tag::terminal,
                       typename boost::enable_if< 
                         boost::function_types::is_function_pointer<
                           typename proto::result_of::value<A>::type
                           > >::type
                       >
  {
    typedef typename boost::remove_pointer< 
      typename proto::result_of::value< A >::type
      >::type 
    function_type;
    
    typedef typename function_input< function_type >::type
    input_type;

    typedef typename function_output< function_type >::type
    output_type;
  };
  
  template< class A >
  struct arrow_traits< A, proto::tag::shift_right >
  {
    typedef typename proto::result_of::left< A >::type   left_type;
    typedef typename proto::result_of::right< A >::type  right_type;

    typedef arrow_traits< left_type >   left_traits;
    typedef arrow_traits< right_type >  right_traits;
    
    BOOST_MPL_ASSERT_MSG( ( boost::is_convertible<
                            typename left_traits::output_type,
                            typename right_traits::input_type
                            >::value ),
                          COMPOSING_INCOMPATIBLE_ARROWS,    
                          ( typename left_traits::output_type,
                            typename right_traits::input_type )
                          );
    
    typedef typename left_traits::input_type    input_type;
    typedef typename right_traits::output_type  output_type;
  };

  template< class A >
  struct arrow_traits< A, proto::tag::comma >
  {
    typedef typename proto::result_of::left< A >::type   left_type;
    typedef typename proto::result_of::right< A >::type  right_type;

    typedef arrow_traits< left_type >   left_traits;
    typedef arrow_traits< right_type >  right_traits;
    
    typedef typename
    fusion::result_of::as_vector<
      typename fusion::result_of::join<
        typename left_traits::input_type,
        typename right_traits::input_type
        >::type
      >::type
    input_type;
    
    typedef typename
    fusion::result_of::as_vector<
      typename fusion::result_of::join<
        typename left_traits::output_type,
        typename right_traits::output_type
        >::type
      >::type
    output_type;
  };

  template< class A >
  struct arrow_traits< A, proto::tag::logical_and >
  {
    typedef typename proto::result_of::left< A >::type   left_type;
    typedef typename proto::result_of::right< A >::type  right_type;

    typedef arrow_traits< left_type >   left_traits;
    typedef arrow_traits< right_type >  right_traits;

    BOOST_MPL_ASSERT_MSG( ( boost::is_same<
                            typename left_traits::input_type,
                            typename right_traits::input_type
                            >::value ),
                          FANOUT_WITH_INCOMPATIBLE_ARROWS,
                          ( typename left_traits::input_type,
                            typename right_traits::input_type )
                          );
    
    typedef typename
    fusion::result_of::as_vector<
      typename fusion::result_of::join<
        typename left_traits::output_type,
        typename right_traits::output_type
        >::type
      >::type
    output_type;
  };
}

#endif

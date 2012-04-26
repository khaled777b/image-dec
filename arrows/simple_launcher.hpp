#ifndef ARROWS_SIMPLE_LAUNCHER_HPP
#define ARROWS_SIMPLE_LAUNCHER_HPP

#include <boost/utility/enable_if.hpp>

#include <boost/function_types/function_type.hpp>
 
#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/algorithm/transformation/push_front.hpp>
#include <boost/fusion/functional/adapter/fused.hpp>
#include <boost/fusion/functional/adapter/unfused_typed.hpp>
#include <boost/fusion/functional/generation/make_fused.hpp>
#include <boost/fusion/functional/generation/make_fused_function_object.hpp>
#include <boost/fusion/view/iterator_range.hpp>

#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/fusion/include/make_fused.hpp>
#include <boost/fusion/include/make_fused_function_object.hpp>
#include <boost/fusion/include/iterator_range.hpp>

#include "arrow.hpp"

namespace arrows 
{

  namespace 
  {

    struct simple_evaluator
    {
      template< typename Expr,
                typename Tag = typename proto::tag_of< Expr >::type,
                typename Enable = void >
      struct eval;
    
      template< typename Expr >
      struct eval< Expr, proto::tag::terminal >
      {
        typedef typename arrow_traits< Expr >::input_type   input_type;
        typedef typename arrow_traits< Expr >::output_type  output_type;

        template< bool empty_output, typename dummy = void >
        struct arr;

        template< typename dummy >
        struct arr< false, dummy >
        {
          typedef output_type result_type;
          
          typedef typename fusion::result_of::make_fused<
            typename proto::result_of::value< Expr >::type
            >::type
          fused_function_type;

          arr( const Expr& expr ) :
            m_fused( fusion::make_fused( proto::value( expr ) ) )
          {}
          
          result_type operator() ( const input_type& x )
          {
            return fusion::make_vector( m_fused( x ) );
          }
        private:
          fused_function_type m_fused;
        };

        template< typename dummy >
        struct arr< true, dummy >
        {
          typedef output_type result_type;
          
          typedef typename fusion::result_of::make_fused<
            typename proto::result_of::value< Expr >::type
            >::type
          fused_function_type;

          arr( const Expr& expr ) :
            m_fused( fusion::make_fused( proto::value( expr ) ) )
          {}
          
          result_type operator() ( const input_type& x )
          {
            m_fused( x );
            return fusion::vector0<>();
          }
        private:
          fused_function_type m_fused;
        };

        typedef arr< fusion::result_of::empty< output_type >::value > result_type;
        
        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return result_type( expr );
        }
      };

      template< typename Expr >
      struct eval< Expr, proto::tag::shift_right >
      {
        typedef typename proto::result_of::left< Expr >::type      left_arrow;
        typedef typename proto::result_of::right< Expr >::type     right_arrow;
        
        typedef typename arrow_traits< left_arrow >::input_type    left_input;
        typedef typename arrow_traits< left_arrow >::output_type   left_output;

        typedef typename arrow_traits< right_arrow >::input_type   right_input;
        typedef typename arrow_traits< right_arrow >::output_type  right_output;

        struct compose 
        {
          // REQUIREMENT:   left_output == right_input
          typedef left_input    input_type;
          typedef right_output  output_type;

          typedef output_type result_type;

          compose( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}
          
          output_type operator() ( const input_type& x )
          {
            return proto::eval( m_right, m_ctx )
              ( proto::eval( m_left, m_ctx )( x ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };
        
        typedef compose result_type;

        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return compose( expr, ctx );
        }
      };

      template< typename Expr >
      struct eval< Expr, proto::tag::comma >
      {
        typedef typename proto::result_of::left< Expr >::type      left_arrow;
        typedef typename proto::result_of::right< Expr >::type     right_arrow;
        
        typedef typename arrow_traits< left_arrow >::input_type    left_input;
        typedef typename arrow_traits< left_arrow >::output_type   left_output;

        typedef typename arrow_traits< right_arrow >::input_type   right_input;
        typedef typename arrow_traits< right_arrow >::output_type  right_output;

        template< bool left_empty, bool right_empty, typename dummy = void >
        struct pair;

        template< typename dummy >
        struct pair< false, false, dummy >
        {
          typedef typename 
          fusion::result_of::as_vector< 
            typename fusion::result_of::join< left_input, right_input >::type
            >::type
          input_type;
          
          typedef typename 
          fusion::result_of::as_vector< 
            typename fusion::result_of::join< left_output, right_output >::type
            >::type
          output_type;

          typedef output_type result_type;

          pair( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}

          output_type operator() ( const input_type& x )
          {
            typedef typename
              fusion::result_of::size< left_input >::type
              left_size;

            typedef typename fusion::result_of::begin< input_type >::type
              begin_iter;
          
            typedef typename fusion::result_of::advance< begin_iter, left_size >::type
              mid_iter;
            
            typedef typename fusion::result_of::end< input_type >::type
              end_iter;

            typedef fusion::iterator_range< begin_iter, mid_iter >
              left_range;
            
            typedef fusion::iterator_range< mid_iter, end_iter >
              right_range;

            begin_iter begin = fusion::begin( x );
            mid_iter   mid   = fusion::advance( x, left_size::value );
            end_iter   end   = fusion::end( x );
            
            left_range   first( begin, mid );
            right_range  second( mid, end );
            
            return fusion::as_vector( fusion::join( proto::eval( m_left,  m_ctx )( first ),
                                                    proto::eval( m_right, m_ctx )( second ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };

        template< typename dummy >
        struct pair< true, false, dummy >
        {
          typedef right_input input_type;
          
          typedef typename 
          fusion::result_of::as_vector< 
            typename fusion::result_of::join< left_output, right_output >::type
            >::type
          output_type;

          typedef output_type result_type;

          pair( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}

          output_type operator() ( const input_type& x )
          {
            return fusion::as_vector( fusion::join( proto::eval( m_left,  m_ctx )( fusion::vector0<>() ),
                                                    proto::eval( m_right, m_ctx )( x ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };
        
        template< typename dummy >
        struct pair< false, true, dummy >
        {
          typedef left_input input_type;
          
          typedef typename 
          fusion::result_of::as_vector< 
            typename fusion::result_of::join< left_output, right_output >::type
            >::type
          output_type;

          typedef output_type result_type;

          pair( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}

          output_type operator() ( const input_type& x )
          {
            return fusion::as_vector( fusion::join( proto::eval( m_left,  m_ctx )( x ),
                                                    proto::eval( m_right, m_ctx )( fusion::vector0<>() ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };

        template< typename dummy >
        struct pair< true, true, dummy >
        {
          typedef typename fusion::vector0<> input_type;
          
          typedef typename 
          fusion::result_of::as_vector< 
            typename fusion::result_of::join< left_output, right_output >::type
            >::type
          output_type;

          typedef output_type result_type;

          pair( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}

          output_type operator() ( const input_type& x )
          {
            return fusion::as_vector( fusion::join( proto::eval( m_left,  m_ctx )( fusion::vector0<>() ),
                                                    proto::eval( m_right, m_ctx )( fusion::vector0<>() ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };
          
        typedef pair< fusion::result_of::empty< left_input >::value,
                      fusion::result_of::empty< right_input >::value >
        result_type;

        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return result_type( expr, ctx );
        }
      };

      template< typename Expr >
      struct eval< Expr, proto::tag::logical_and >
      {
        typedef typename proto::result_of::left< Expr >::type      left_arrow;
        typedef typename proto::result_of::right< Expr >::type     right_arrow;
        
        typedef typename arrow_traits< left_arrow >::input_type    left_input;
        typedef typename arrow_traits< left_arrow >::output_type   left_output;

        typedef typename arrow_traits< right_arrow >::input_type   right_input;
        typedef typename arrow_traits< right_arrow >::output_type  right_output;

        struct fanout
        {
          // REQUIREMENT:   left_input == right_input
          typedef left_input  input_type;
          
          typedef typename 
          fusion::result_of::as_vector<
            typename fusion::result_of::join< left_output, right_output >::type
            >::type
          output_type;

          typedef output_type result_type;


          fanout( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}
          
          output_type operator() ( const input_type& x )
          {
            return fusion::as_vector( fusion::join( proto::eval( m_left,  m_ctx )( x ),
                                                    proto::eval( m_right, m_ctx )( x ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };
        
        typedef fanout result_type;

        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return fanout( expr, ctx );
        }
      };

    };
  }

  template< typename _Arrow >
  struct simple_launcher :
    fusion::unfused_typed< 
    typename proto::result_of::eval< const _Arrow, simple_evaluator >::type,
    typename arrow_traits< _Arrow >::input_type
    >
  {
    typedef fusion::unfused_typed< 
      typename proto::result_of::eval< const _Arrow, simple_evaluator >::type,
      typename arrow_traits< _Arrow >::input_type
      >
    unfused_function_type;

    simple_launcher( const _Arrow& arrow ) :
      unfused_function_type( proto::eval( arrow, simple_evaluator() ) )
    {}
  };

  template< typename _Arrow >
  simple_launcher< _Arrow > launch_simple( const _Arrow& arrow ) 
  {
    return simple_launcher<_Arrow>( arrow );
  }
  
}

#endif

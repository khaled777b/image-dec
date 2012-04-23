#ifndef ARROWS_SIMPLE_LAUNCHER_HPP
#define ARROWS_SIMPLE_LAUNCHER_HPP

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

        struct arr
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

        typedef arr result_type;
        
        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return arr( expr );
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

        struct pair
        {
          typedef typename 
          fusion::result_of::join< left_input, right_input >::type
          input_type;
          
          typedef typename 
          fusion::result_of::join< left_output, right_output >::type
          output_type;

          typedef output_type result_type;


          pair( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}

          left_input first( const input_type& x )
          {
            typedef typename
              fusion::result_of::size< left_input >::type
              Size;
            
            typedef typename
              fusion::result_of::begin< input_type >::type
              Begin;
            
            typedef typename
              fusion::result_of::advance< Begin, Size >::type
              End;

            typedef fusion::iterator_range< Begin, End > Range;
            
            Begin begin( x );
            End   end( x );
            
            return left_input( Range( begin, end ) );
          }

          right_input second( const input_type& x )
          {
            typedef typename
              fusion::result_of::size< left_input >::type
              LSize;
            
            typedef typename
              fusion::result_of::begin< input_type >::type
              LBegin;

            typedef typename
              fusion::result_of::size< right_input >::type
              Size;

            typedef typename
              fusion::result_of::advance< LBegin, LSize >::type
              Begin;
            
            typedef typename
              fusion::result_of::advance< Begin, Size >::type
              End;

            typedef fusion::iterator_range< Begin, End > Range;
            
            Begin begin( x );
            End   end( x );
            
            return right_input( Range( begin, end ) );
          }
          
          output_type operator() ( const input_type& x )
          {
            return fusion::join( proto::eval( m_left,  m_ctx )( first( x ) ),
                                 proto::eval( m_right, m_ctx )( second( x ) ) );
          }
        private:
          const simple_evaluator&  m_ctx;
          const left_arrow&        m_left;
          const right_arrow&       m_right;
        };
        
        typedef pair result_type;

        result_type operator() ( const Expr& expr, const simple_evaluator& ctx )
        {
          return pair( expr, ctx );
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
          fusion::result_of::join< left_output, right_output >::type
          output_type;

          typedef output_type result_type;


          fanout( const Expr& expr, const simple_evaluator& ctx ) :
            m_ctx( ctx ),
            m_left( proto::left( expr ) ), 
            m_right( proto::right( expr ) )
          {}
          
          output_type operator() ( const input_type& x )
          {
            return fusion::join( proto::eval( m_left,  m_ctx )( x ),
                                 proto::eval( m_right, m_ctx )( x ) );
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

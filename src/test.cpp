
#include <iostream>

#include "metamath.hpp"

int main()
{
  std::cout << "2^10 = " << pow2< mpl::size_t<10> >::type::value << std::endl;
  std::cout << "2*100 = " << times2< mpl::size_t<100> >::type::value
            << std::endl;

  return 0;
}

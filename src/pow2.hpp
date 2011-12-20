
template< size_t N >
struct pow2
{
  static const size_t value = 2 * pow2<N-1>::value;
};

template<>
struct pow2<0>
{
  static const size_t value = 1;
};

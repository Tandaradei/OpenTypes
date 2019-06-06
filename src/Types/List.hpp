#ifndef TYPES_LIST_HPP
#define TYPES_LIST_HPP

#include "src/open_types.hpp"

#if TEMPLATE_TYPES_ENABLED

TEMPLATE_TYPE(Pair, K, V)
//TEMPLATE_ATTR1(key, Pair, K)

TEMPLATE_TYPE(List, T)
TEMPLATE_ATTR1(size, List, List<T>, size_t, T)

#endif




#endif // TYPES_LIST_HPP

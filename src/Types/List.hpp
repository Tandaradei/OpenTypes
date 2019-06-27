#ifndef TYPES_LIST_HPP
#define TYPES_LIST_HPP

#include "src/open_types.hpp"

TEMPLATE_TYPE(List, typename T)
TEMPLATE_ATTR1(head, List, T, T, typename T)
TEMPLATE_ATTR1(tail, List, T, List<T>, typename T)


TEMPLATE_TYPE(Pair, typename K COMMA typename V)
TEMPLATE_ATTR1(key, Pair, K COMMA V, K, typename K COMMA typename V)
TEMPLATE_ATTR1(value, Pair, K COMMA V, V, typename K COMMA typename V)

#endif // TYPES_LIST_HPP

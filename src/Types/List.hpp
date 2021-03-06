#ifndef TYPES_LIST_HPP
#define TYPES_LIST_HPP

#include "src/open_types.hpp"

OT_TEMPLATE_TYPE(List, T, typename T)
OT_TEMPLATE_ATTR1(head, List, T, T, typename T)
OT_TEMPLATE_ATTR1(tail, List, T, List<T>, typename T)

OT_TEMPLATE_TYPE(Pair, K COMMA V, typename K COMMA typename V)
OT_TEMPLATE_ATTR1(key, Pair, K COMMA V, K, typename K COMMA typename V)
OT_TEMPLATE_ATTR1(value, Pair, K COMMA V, V, typename K COMMA typename V)

OT_TEMPLATE_TYPE_SERIALIZE(
	Pair, K COMMA V, typename K COMMA typename V,
	t[key<K COMMA V>] COMMA t[value<K COMMA V>],
	t.createReference(key<K COMMA V>) COMMA t.createReference(value<K COMMA V>)
)
#endif // TYPES_LIST_HPP

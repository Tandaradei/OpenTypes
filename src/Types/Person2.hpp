#ifndef TYPES_PERSON2_HPP
#define TYPES_PERSON2_HPP

#include "src/open_types2.hpp"

enum class Grade {
	A,
	B,
	C,
	D,
	F
};

OT_TYPE(Person)
OT_ATTR1(name, Person, std::string)
OT_ATTR1(age, Person, int)
OT_ATTRN(grades, Person, Grade);

OT_TYPE(Car)
OT_ATTR1(name, Car, std::string)
OT_REL1N(cars, Person, Car, owner)

#endif // TYPES_PERSON2_HPP

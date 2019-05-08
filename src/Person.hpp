#ifndef MY_TYPES_HPP
#define MY_TYPES_HPP

#include <set>

#include "open_types.hpp"

TYPE(Skill)
ATTR1(name, Skill, std::string)
ATTR1(value, Skill, float)

TYPE(Person)
ATTR1(name, Person, std::string)
ATTR1(age, Person, int)
ATTRN(skills, Person, Skill)
ATTRN(tags, Person, std::string)

#endif // MY_TYPES_HPP

#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include "src/open_types.hpp"
#include "src/Types/Skill.hpp"

TYPE(Person)
ATTR1(name, Person, std::string)
ATTR1(age, Person, int)
ATTRN(skills, Person, Skill)

#endif // TYPES_PERSON_HPP

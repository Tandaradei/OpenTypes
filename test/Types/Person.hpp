#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include "src/open_types.hpp"
#include "test/Types/Skill.hpp"

TYPE(Person)
ATTR1(name, Person, std::string)
ATTR1(age, Person, int)
ATTRN(skills, Person, Skill)
ATTRN(tags, Person, std::string)

#endif // TYPES_PERSON_HPP

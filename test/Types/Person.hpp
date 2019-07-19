#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include "src/open_types2.hpp"
#include "test/Types/Skill.hpp"

OT_TYPE(Person)
OT_ATTR1(name, Person, std::string)
OT_ATTR1(age, Person, int)
OT_ATTRN(tags, Person, std::string)

#endif // TYPES_PERSON_HPP

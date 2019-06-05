#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include "src/open_types.hpp"
#include "src/Types/Skill.hpp"
#include "src/Types/Car.hpp"

TYPE(Person)
ATTR1(name, Person, std::string)
ATTR1(age, Person, int)
ATTRN(skills, Person, Skill)
REL1N(cars, Person, Car, owner)

REL1(spouse, Person)
RELN(friends, Person)

#endif // TYPES_PERSON_HPP

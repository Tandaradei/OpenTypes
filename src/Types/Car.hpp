#ifndef TYPES_CAR_HPP
#define TYPES_CAR_HPP

#include "src/open_types.hpp"

TYPE(Engine)
ATTR1(name, Engine, std::string)

TYPE(Car)
ATTR1(model, Car, std::string)
REL11(engine, Car, Engine, car)

#endif // TYPES_PERSON_HPP

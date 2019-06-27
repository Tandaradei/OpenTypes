#ifndef TYPES_CAR_HPP
#define TYPES_CAR_HPP

#include "src/open_types.hpp"
#include "test/Types/Person.hpp"

TYPE(Engine)
ATTR1(kmRun, Engine, int)

TYPE(Car)
ATTR1(model, Car, std::string)
REL11(engine, Car, Engine, car)
REL1N(owner, Car, Person, cars)

#endif // TYPES_CAR_HPP

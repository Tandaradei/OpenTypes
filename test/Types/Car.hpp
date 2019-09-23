#ifndef TYPES_CAR_HPP
#define TYPES_CAR_HPP

#include "src/open_types.hpp"
#include "test/Types/Person.hpp"

OT_TYPE(Engine)
OT_ATTR1(kmRun, Engine, int)

OT_TYPE(Car)
OT_ATTR1(model, Car, std::string)
OT_REL11(engine, Car, Engine, car)
OT_REL1N(owner, Car, Person, cars)

#endif // TYPES_CAR_HPP

#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include <iostream>

#include "src/open_types.hpp"

enum class Grade {
    A,
    B,
    C,
	D,
    F = 5
};

OT_TYPE(Person)
OT_ATTR1(name, Person, std::string)
void printName(const Person& p) {
	std::cout << p[name] << "\n";
}
OT_ATTR1(age, Person, int)
OT_ATTRN(grades, Person, Grade);
OT_RELNN(teachers, Person, Person, students);

OT_TYPE(Car)
OT_ATTR1(name, Car, std::string)
OT_REL1N(cars, Person, Car, owner)

#endif // TYPES_PERSON_HPP

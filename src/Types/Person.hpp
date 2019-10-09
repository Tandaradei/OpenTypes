#ifndef TYPES_PERSON_HPP
#define TYPES_PERSON_HPP

#include <iostream>

#include "src/open_types.hpp"

enum class Grade
{
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
OT_REL1(spouse, Person);
OT_RELN(friends, Person);
void printFriends(const Person& p) {
	std::cout << "friends of " << p[name] << ": \n";
	for (auto f : p[friends]) {
		std::cout << " " << f[name] << "\n";
	}
}

OT_TYPE_SERIALIZE(
	Person,
	t[name] COMMA t[age] COMMA t[grades],
	t.createReference(name) COMMA t.createReference(age) COMMA t.createReference(grades)
);

OT_TYPE(Car)
OT_ATTR1(name, Car, std::string)
OT_REL1N(cars, Person, Car, owner)

#endif // TYPES_PERSON_HPP

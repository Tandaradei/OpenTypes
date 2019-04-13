#include <iostream>

#include "open_types.hpp"


int main() {
    TYPE(Person);
    Person p;
    ATTR1(name, Person, std::string);

	std::getchar();

	return 0;
}

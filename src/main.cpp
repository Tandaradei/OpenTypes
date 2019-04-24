#include <iostream>

#include "open_types.hpp"


int main() {
	TYPE(Person)
	TYPE(Person2)

	Person p;
	ATTR1(name, Person, std::string)

	ot::AttributeAccessor<std::string>* name = ot::AttributeAccessor<std::string>::get("Person");
	name->getValueFor(&p);

	std::getchar();

	return 0;
}

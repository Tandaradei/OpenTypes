#ifndef HELPER2_HPP
#define HELPER2_HPP

#include "src/Types/Person.extended.hpp"

void tag(Person p, std::string value) {
	p(tags, 0, value);
}

#endif // HELPER2_HPP
#ifndef HELPER_HPP
#define HELPER_HPP

#include "src/Types/Person.extended.hpp"

void printSkills(Person p) {
	std::cout << "skills of " << p[name] << ": " << std::endl;
    for (auto skill : p[skills]) {
		std::cout << skill[name] << ": " << skill[value] * 100.0f << "%" << std::endl;
	}
}

void printTags(Person p) {
	std::cout << "tags of " << p[name] << ": " << std::endl;
	for (auto tag : p[tags]) {
		std::cout << tag << std::endl;
	}
}

#endif // HELPER_HPP

#ifndef HELPER2_HPP
#define HELPER2_HPP

#include "src/Types/Person.extended.hpp"
#include "src/Types/List.hpp"

void tag(Person p, std::string value) {
	p(tags, 0, value);
}

template <typename T>
void printList(List<T> list) {
	std::cout << list[head<T>];
	if (list[tail<T>] != ot::nil) {
		std::cout << ", ";
		printList(tail(list));
	}
	else {
		std::cout << "\n";
	}
}

#endif // HELPER2_HPP
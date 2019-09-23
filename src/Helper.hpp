#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>

#include "src/Types/Person.hpp"
#include "src/Types/List.hpp"

template <typename T>
void printList(List<T> list) {
	std::cout << list[head<T>];
    if (list[tail<T>] != nullptr) {
		std::cout << ", ";
		printList(tail(list));
	}
	else {
		std::cout << "\n";
	}
}

#endif // HELPER_HPP

#ifndef HELPER2_HPP
#define HELPER2_HPP

#include <iostream>

#include "src/Types/Person2.hpp"
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

#endif // HELPER2_HPP

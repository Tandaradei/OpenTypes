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

void printGrades(Person p) {
	bool first = true;
	std::cout << "grades: ";
	for (const auto& grade : p[grades]) {
		if (first) {
			first = false;
		}
		else {
			std::cout << ", ";
		}
		std::cout << static_cast<char>(static_cast<int>(grade) + static_cast<int>('A'));
	}
	std::cout << "\n";
}

#endif // HELPER_HPP

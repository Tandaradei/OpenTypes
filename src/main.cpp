#include <iostream>

#include "my_types.hpp"

int main() {
    Person p;
    std::cout << "name of p: " << p[name] << std::endl;
    std::cout << "age of p: " << age(p) << std::endl;
    p(name, std::string("Hans"));
    age(p, 37);
    std::cout << "name of p: " << p[name] << std::endl;
    std::cout << "age of p: " << age(p) << std::endl;

	return 0;
}

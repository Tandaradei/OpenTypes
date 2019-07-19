#include <iostream>

//#include "src/Types/Person.extended.hpp"
#include "src/Types/List.hpp"

#include "src/Types/Person2.hpp"

//#include "Helper.hpp"
#include "Helper2.hpp"

int main() {
	Person hans = Person::create(); // Create Person object
	Person hans_ref = hans; // Create Person variable referencing hans
	std::cout << "name of hans: " << hans[name] << std::endl; // Read empty attribute name of hans
	std::cout << "age of hans: " << age(hans) << std::endl; // Read empty attribute age of hans
	hans(name, std::string("Hans"))(age, 37);  // Set attributes name and age of hans
	std::cout << "name of hans: " << name(hans) << std::endl; // Read set attribute name of hans
	std::cout << "age of hans: " << hans[age] << std::endl; // Read set attribute age of hans
	auto printChildAges = [](Person p) {
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
		std::cout << std::endl;
	};
	printChildAges(hans);
	hans(grades, Grade::A)(grades, Grade::C)(grades, Grade::C)(grades, Grade::F);
	printChildAges(hans);
	hans.deleteByValue(grades, ot::DeleteByValue<Grade>(Grade::C));
	printChildAges(hans);
	Car myCar(name, std::string("BMW"));
	myCar(owner, hans);
	std::cout << "first car of Hans: " << hans[cars][0][name] << std::endl;
	std::cout << "owner of BMW: " << name(owner(myCar)) << std::endl;
	{
		Car myCar2(name, std::string("Audi"));
		hans(cars, myCar2);
		std::cout << "first car of Hans: " << hans[cars][0][name] << std::endl;
		std::cout << "second car of Hans: " << hans[cars][1][name] << std::endl;
		std::cout << "owner of BMW: " << name(owner(myCar)) << std::endl;
		std::cout << "owner of Audi: " << name(owner(myCar2)) << std::endl;
	}
	std::cout << "first car of Hans: " << hans[cars][0][name] << std::endl;
	std::cout << "second car of Hans: " << hans[cars][1][name] << std::endl;
	std::cout << "owner of BMW: " << name(owner(myCar)) << std::endl;
    List<int> list = List<int>(head, 3)(tail, List<int>(head, 2)(tail, List<int>(head, 8)));
    list[head<int>];
    head(list);

    printList(list);
	Pair<std::string, int> person = Pair<std::string, int>::create();
	person(key, std::string("Hans"));
	person(value, 5);
	printf("%s", person[key<std::string, int>].c_str());
	printf(" %d\n", person[value<std::string, int>]);
	return 0;
}

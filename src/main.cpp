#include <iostream>

#include "src/Types/Person.extended.hpp"
#include "src/Types/List.hpp"

#include "Helper.hpp"
#include "Helper2.hpp"

int main() {
    Person hans = ot::empty(); // Create Person object
	Person hans_ref = hans; // Create Person variable referencing hans
    std::cout << "name of hans: " << hans[name] << std::endl; // Read empty attribute name of hans
    std::cout << "age of hans: " << age(hans) << std::endl; // Read empty attribute age of hans
	hans(name, std::string("Hans"))(age, 37);  // Set attributes name and age of hans
    std::cout << "name of hans: " << name(hans) << std::endl; // Read set attribute name of hans
    std::cout << "age of hans: " << hans[age] << std::endl; // Read set attribute age of hans
	printSkills(hans); // Read empty list of attributes
    hans(skills, Skill(name, std::string("Java"))(value, 0.1f)); // Add new value to attribute list
	hans_ref(skills, Skill(name, std::string("C++"))(value, 0.35f)); // Add new value to attribute list
	printSkills(hans_ref); // Read filled list of attributes using reference
	hans(tags, std::string("Software Engineer"))(tags, std::string("Software Architect")); // Add two values to attribute list
	printTags(hans_ref); // Read filled list of attributes using reference
	hans(tag, std::string("Quality Assurance")); // Update a value of attribute list using helper function
	printTags(hans); // Read filled list of attributes 
	hans -= REMOVE_VALUE(tags, Person, std::string, "Software Architect"); // Remove first "Software Architect" from tags of hans
	printTags(hans); // Read filled list of attributes 
	std::cout << "get undefined skill name from skills of hans: " << hans[skills][4][name] << std::endl; // Try to read undefined item of attribute list
	//hans -= name; // Remove attribute value
	std::cout << "name of hans: " << hans[name] << std::endl; // Read removed attribute

	Car audi(model, std::string("Audi"));
	Engine v6(car, audi);
	v6(name, std::string("V6 Biturbo"));
	Car bmw(model, std::string("BMW"));
	Engine v8(name, std::string("V8"));
	bmw(engine, v8);
	std::cout << "Car models of hans: " << std::endl;
	for (auto car : hans[cars]) {
		std::cout << "* " << car[model] << std::endl;
	}
	std::cout << "Owner name of audi: " << audi[owner][name] << std::endl;
	std::cout << "Owner name of bmw: " << bmw[owner][name] << std::endl;
	hans(cars, audi);
	printCars(hans);
	std::cout << "Owner name of audi: " << audi[owner][name] << std::endl;
	std::cout << "Owner name of bmw: " << bmw[owner][name] << std::endl;
	bmw(owner, hans);
	printCars(hans);
	std::cout << "Owner name of audi: " << audi[owner][name] << std::endl;
	std::cout << "Owner name of bmw: " << bmw[owner][name] << std::endl;

	std::cout << v6[name] << " is build into " << v6[car][model] << std::endl;

	Person peter(name, std::string("Peter"));
	Person jens(name, std::string("Jens"));

	hans(friends, peter);
	hans(friends, jens);

	printFriends(hans);
	printFriends(peter);
	printFriends(jens);

#if TEMPLATE_TYPES_ENABLED
	List<int> list = ot::empty();
	list(head, 3)(tail, List<int>(head, 2));

	printList(list);

	Pair<std::string, int> person = ot::empty();
	person(key, std::string("Hans"));
	person(value, 5);
	printf("%s", person[key<std::string, int>].c_str());
	printf(" %d\n", person[value<std::string, int>]);
#endif

	return 0;
}

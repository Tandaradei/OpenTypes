#include <iostream>

#include "src/Types/Person.extended.hpp"

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
	hans_ref(skills, Skill(true)); // Add new value to attribute list using reference
	hans_ref[skills][1](name, std::string("C++"))(value, 0.35f); // Set values to new list item using reference
	hans(skills, Skill(true)); // Add new value to attribute list
	printSkills(hans_ref); // Read filled list of attributes using reference
	hans(tags, std::string("Software Engineer"))(tags, std::string("Software Architect")); // Add two values to attribute list
	printTags(hans_ref); // Read filled list of attributes using reference
	hans(tag, std::string("Quality Assurance")); // Update a value of attribute list using helper function
	printTags(hans); // Read filled list of attributes 
	hans -= REMOVE_VALUE(tags, Person, std::string, std::string("Software Architect")); // Remove first "Software Architect" from tags of hans
	printTags(hans); // Read filled list of attributes 
	std::cout << "get undefined skill name from skills of hans: " << hans[skills][4][name] << std::endl; // Try to read undefined item of attribute list
	std::cout << "first skill of " << hans[name] << ": " << hans_ref[skill][name] << std::endl; // Read attribute of item in attribute list using reference and helper function
	hans -= name; // Remove attribute value
    //hans[skills] -= skill1;
	std::cout << "name of hans: " << hans[name] << std::endl; // Read removed attribute

	return 0;
}

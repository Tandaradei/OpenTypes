#include <iostream>

#include "Person.hpp"

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

Skill skill(Person p) {
	return p[skills][0];
}

void tag(Person p, std::string value) {
	p(tags, 0, value);
}

int main() {
    Person p = ot::empty();
	Person p2 = ot::empty();
    std::cout << "name of p: " << name(p) << std::endl;
    std::cout << "age of p: " << age(p) << std::endl;
    p(name, std::string("Hans"))(age, 37);
    std::cout << "name of p: " << name(p) << std::endl;
    std::cout << "age of p: " << age(p) << std::endl;
	printSkills(p);
	p(skills, Skill(ot::empty()));
	p[skills][0](name, std::string("Java"))(value, 0.1f);
	p(skills, Skill(true));
	p[skills][1](name, std::string("C++"))(value, 0.35f);
	p(skills, Skill(true));
	printSkills(p);
	p(tags, std::string("Software Engineer"))(tags, std::string("Software Architect"));
	printTags(p);
	printSkills(p2);
	p(tag, std::string("Quality Assurance"));
	printTags(p);
	std::cout << "get undefined skill name from skills of p: " << p[skills][4][name] << std::endl;

	std::cout << "first skill of " << p[name] << ": " << p[skill][name] << std::endl;

	p -= name;
	std::cout << "name of p: " << p[name] << std::endl;
	return 0;
}

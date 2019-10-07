#include <fstream>
#include <iostream>

#include "cereal/archives/xml.hpp"

#include "src/Types/Person.hpp"
#include "src/Types/List.hpp"

#include "src/Helper.hpp"


int main() {
	Person personLoaded = Person::create();
	Pair<std::string, int> pairLoaded = Pair<std::string, int>::create();

	{
		std::ifstream is("data.xml");
		if (is.is_open()) {
			cereal::XMLInputArchive archive(is);

			archive(ot_cereal::PersonLoad(personLoaded), ot_cereal::PairLoad<std::string, int>(pairLoaded));
		}
	}

	std::cout << "name of loaded person: " << personLoaded[name] << std::endl;
	std::cout << "age of loaded person: " << age(personLoaded) << std::endl;
	printGrades(personLoaded);

	printf("%s", pairLoaded[key<std::string, int>].c_str());
	printf(" %d\n", pairLoaded[value<std::string, int>]);

	Person hans = Person::create(); // Create Person object
	Person hans_ref = hans; // Create Person variable referencing hans
	std::cout << "name of hans: " << hans[name] << std::endl; // Read empty attribute name of hans
	std::cout << "age of hans: " << age(hans) << std::endl; // Read empty attribute age of hans
	hans(name, std::string("Hans"))(age, 37);  // Set attributes name and age of hans
	std::cout << "name of hans: "; hans[printName]; // Read set attribute name of hans
	std::cout << "age of hans: " << hans[age] << std::endl; // Read set attribute age of hans
	
	printGrades(hans);
	hans(grades, Grade::A)(grades, Grade::C)(grades, Grade::C)(grades, Grade::F);
	printGrades(hans);
	hans.deleteByValue(grades, ot::DeleteByValue<Grade>(Grade::C));
	printGrades(hans);
	Car myCar(name, std::string("BMW"));
	myCar(owner, hans);
	std::cout << "first car of Hans: " << hans[cars][0][name] << std::endl;
	std::cout << "owner of BMW: " << name(owner(myCar)) << std::endl;
	{
		Car myCar2(name, std::string("Audi"));
		hans(cars, myCar2);
		Car myCar3(name, std::string("VW"));
		hans(cars, myCar3);
		std::cout << "first car of Hans: " << hans[cars][0][name] << std::endl;
		std::cout << "second car of Hans: " << hans[cars][1][name] << std::endl;
		std::cout << "third car of Hans: " << hans[cars][2][name] << std::endl;
		std::cout << "owner of BMW: " << name(owner(myCar)) << std::endl;
		std::cout << "owner of Audi: " << name(owner(myCar2)) << std::endl;
		std::cout << "owner of VW: " << name(owner(myCar3)) << std::endl;
		hans.deleteByValue(cars, ot::DeleteByValue<Car>(myCar2)); // delete myCar2 from cars list
		std::cout << "second car of Hans: " << hans[cars][1][name] << std::endl; // should be myCar2 now -> 'VW'
	}
	// myCar3 / 'VW' should be out of scope and therefore auto deleted from cars list
	ot::vector<Car> hansCars = hans[cars];
	std::cout << "# cars of Hans: " << hansCars.size() << std::endl;
	Car first = hansCars[0];
	std::cout << "first car of Hans: " << first[name] << std::endl;
	Car second = hans[cars][1]; // should be a nil reference
	std::cout << "second car of Hans: " << second[name] << std::endl; 

	Person anna(name, std::string("Anna"));
	hans(spouse, anna);
	std::cout << "spouse of " << hans[name] << ": " << hans[spouse][name] << "\n";
	std::cout << "spouse of " << anna[name] << ": " << anna[spouse][name] << "\n";
	anna -= spouse;
	std::cout << "spouse of " << hans[name] << ": " << hans[spouse][name] << "\n";
	std::cout << "spouse of " << anna[name] << ": " << anna[spouse][name] << "\n";

	Person peter(name, std::string("Peter"));
	anna(friends, hans);
	anna(friends, peter);
	hans[printFriends];
	anna[printFriends];
	peter[printFriends];

	anna.deleteByValue(friends, ot::DeleteByValue<Person>(peter));
	hans[printFriends];
	anna[printFriends];
	peter[printFriends];

    List<int> list = List<int>(head, 3)(tail, List<int>(head, 2)(tail, List<int>(head, 8)));
    list[head<int>];
    head(list);

    printList(list);
	Pair<std::string, int> person = Pair<std::string, int>::create();
	person(key, std::string("Hans"));
	person(value, 5);
	printf("%s", person[key<std::string, int>].c_str());
	printf(" %d\n", person[value<std::string, int>]);

	{
		std::ofstream os("data.xml");
		cereal::XMLOutputArchive archive(os);

		archive(ot_cereal::PersonSave(hans));
		archive(ot_cereal::PairSave<std::string, int>(person));
	}

	return 0;
}

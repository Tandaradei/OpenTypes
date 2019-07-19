#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("attributes", "[attribute]") {
	GIVEN("a nil variable and an object of type 'Person'") {
		Person p_var = Person::nil();
		Person p_obj = Person::create();
		WHEN("attribute 'name' was not set") {
			THEN("reading 'name' should return empty string") {
				REQUIRE(p_var[name] == std::string());
				REQUIRE(name(p_var) == std::string());
				REQUIRE(p_obj[name] == std::string());
				REQUIRE(name(p_obj) == std::string());
			}
		}
		WHEN("attribute 'age' was not set") {
			THEN("reading 'age' should return 0") {
				REQUIRE(p_var[age] == 0);
				REQUIRE(p_obj[age] == 0);
			}
		}
		WHEN("attribute 'name' was set on object") {
			p_obj(name, std::string("Peter"));
			THEN("reading 'name' from object should return 'Peter'") {
				REQUIRE(p_obj[name] == "Peter");
			}
		}
		WHEN("attribute 'name' was set on nil variable") {
			p_var(name, std::string("Hans"));
			THEN("reading 'name' from variable should return empty string") {
				REQUIRE(p_var[name] == std::string());
			}
		}
	}
	GIVEN("an object of type 'Person' and a variable referencing it") {
		Person p_obj = Person::create();
		Person p_var = p_obj;
		WHEN("attribute 'name' was set on object") {
			p_obj(name, std::string("Peter"));
			THEN("reading 'name' from object or variable should return 'Peter'") {
				REQUIRE(p_obj[name] == "Peter");
				REQUIRE(p_var[name] == "Peter");
			}
		}
		WHEN("attribute 'name' was set on variable") {
			p_var(name, std::string("Hans"));
			THEN("reading 'name' from object or variable should return 'Hans'") {
				REQUIRE(p_obj[name] == "Hans");
				REQUIRE(p_var[name] == "Hans");
			}
		}
	}
	GIVEN("an object of type 'Person' with attribute 'name' set on construction and a variable referencing the object") {
		Person p_obj(name, std::string("Peter"));
		Person p_var = p_obj;
		THEN("reading 'name' from object or variable should return 'Peter'") {
			REQUIRE(p_obj[name] == "Peter");
			REQUIRE(p_var[name] == "Peter");
		}
	}
	GIVEN("an object of type 'Person' with attribute 'name' and 'age' set on construction and a variable referencing the object") {
		Person p_obj = Person(name, std::string("Hans"))(age, 40);
		Person p_var = p_obj;
		THEN("reading 'name' and 'age' from object or variable should return 'Hans' and 40") {
			REQUIRE((p_obj[name] == "Hans" && p_obj[age] == 40));
			REQUIRE((p_var[name] == "Hans" && p_var[age] == 40));
		}
	}
	GIVEN("an object of type 'Person' with attribute 'name' and 'age' set on construction and a variable referencing the object") {
		Person p_obj = Person(name, std::string("Hans"))(age, 40);
		Person p_var = p_obj;
		THEN("deleting 'name' from object and then reading 'name' from object or variable should return empty string") {
			p_obj -= name;
			REQUIRE((p_obj[name] == std::string() && p_obj[age] == 40));
			REQUIRE((p_var[name] == std::string() && p_var[age] == 40));
		}
		THEN("deleting 'age' from variable and then reading 'age' from object or variable should return 0") {
			p_var -= age;
			REQUIRE((p_obj[name] == "Hans" && p_obj[age] == 0));
			REQUIRE((p_var[name] == "Hans" && p_var[age] == 0));
		}
	}
	GIVEN("an object of type 'Person' with list-attribute 'tags' and a variable referencing the object") {
		Person p_obj = Person::create();
		Person p_var = p_obj;
		THEN("reading the size of empty list 'tags' should return 0") {
			REQUIRE(p_obj[tags].size() == 0);
			REQUIRE(p_var[tags].size() == 0);
		}
		THEN("reading an element at an arbitary positive position of empty list 'tags' should return empty string") {
			std::vector<size_t> positions = { 0, 1, 5, std::numeric_limits<size_t>::max() };
			for (auto pos : positions) {
				REQUIRE(p_obj[tags][pos] == std::string());
				REQUIRE(p_var[tags][pos] == std::string());
			}
		}
		THEN("reading an element at an arbitary position of empty list 'tags' should return empty string") {
			std::vector<int> positions = { std::numeric_limits<int>::min(), -1, 0, 1, 5, std::numeric_limits<int>::max() };
			for (auto pos : positions) {
				REQUIRE(p_obj[tags][pos] == std::string());
				REQUIRE(p_var[tags][pos] == std::string());
			}
		}
		THEN("adding an element to 'tags' and reading it's size should return 1") {
			p_obj(tags, std::string("Easy"));
			REQUIRE(p_obj[tags].size() == 1);
			REQUIRE(p_var[tags].size() == 1);
		}
		THEN("adding 10 elements to 'tags' and reading it's size should return 10") {
			for (int i = 0; i < 10; i++) {
				p_obj(tags, std::string("Easy"));
			}
			REQUIRE(p_obj[tags].size() == 10);
			REQUIRE(p_var[tags].size() == 10);
		}
		THEN("adding an element to empty list 'tags' at position 5 and reading it's size should return 1") {
			p_obj(tags, 5, std::string("Easy"));
			REQUIRE(p_obj[tags].size() == 1);
			REQUIRE(p_var[tags].size() == 1);
		}
		THEN("adding an element to empty list 'tags' at position 5 and reading it's first element should return 'Easy' while reading the element at position 5 should return empty string") {
			p_obj(tags, 5, std::string("Easy"));
			REQUIRE((p_obj[tags][0] == "Easy" && p_obj[tags][5] == std::string()));
			REQUIRE((p_var[tags][0] == "Easy" && p_var[tags][5] == std::string()));
		}
        GIVEN("the list 'tags' with 5 elements") {
            p_var(tags, std::string("Zero"))
                 (tags, std::string("One"))
                 (tags, std::string("Two"))
                 (tags, std::string("Three"))
                 (tags, std::string("Four"));
            THEN("adding an element 'Five' at position 5") {
                p_obj(tags, 5, std::string("Five"));
                THEN("reading it's size should return 6") {
                    REQUIRE(p_obj[tags].size() == 6);
                    REQUIRE(p_var[tags].size() == 6);
                }
                THEN("reading the element at position 5 should return 'Five'") {
                    REQUIRE(p_obj[tags][5] == "Five");
                    REQUIRE(p_obj[tags][5] == "Five");
                }
            }
            THEN("adding an element 'Six' at position 6") {
                p_obj(tags, 6, std::string("Six"));
                THEN("reading it's size should return 6") {
                    REQUIRE(p_obj[tags].size() == 6);
                    REQUIRE(p_var[tags].size() == 6);
                }
                THEN("reading the element at position 5 should return 'Six'") {
                    REQUIRE(p_obj[tags][5] == "Six");
                    REQUIRE(p_obj[tags][5] == "Six");
                }
                THEN("reading the element at position 6 should return empty string") {
                    REQUIRE(p_obj[tags][6] == "");
                    REQUIRE(p_obj[tags][6] == "");
                }
            }
            THEN("adding an element 'Inserted' at position 3") {
                p_obj(tags, 3, std::string("Inserted"));
                THEN("reading it's size should return 6") {
                    REQUIRE(p_obj[tags].size() == 6);
                }
                THEN("reading the element at position 3 should return 'Inserted'") {
                    REQUIRE(p_obj[tags][3] == "Inserted");
                }
                THEN("reading the element at position 5 should return 'Four'") {
                    REQUIRE(p_obj[tags][5] == "Four");
                }
                THEN("reading the element at position 6 should return empty string") {
                    REQUIRE(p_obj[tags][6] == "");
                }
            }
            THEN("adding an element 'Inserted' at position -1") {
                p_obj(tags, -1, std::string("Inserted"));
                THEN("reading it's size should return 6") {
                    REQUIRE(p_obj[tags].size() == 6);
                }
                THEN("reading the element at position 5 should return 'Inserted'") {
                    REQUIRE(p_obj[tags][5] == "Inserted");
                }
                THEN("reading the element at position -1 should return empty string") {
                    REQUIRE(p_obj[tags][-1] == "");
                }
            }
        }
	}
}

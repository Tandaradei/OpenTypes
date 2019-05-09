#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("attributes", "[attribute]") {
	GIVEN("a nil variable and an object of type 'Person'") {
		Person p_var;
		Person p_obj = ot::empty();
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
		Person p_obj = ot::empty();
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
}

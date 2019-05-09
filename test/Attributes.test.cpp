#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("attributes", "[attribute]") {
	GIVEN("a variable and an object of type 'Person'") {
		Person p_var;
		Person p_obj = ot::empty();
		WHEN("attribute 'name' was not set") {
			THEN("reading 'name' should return empty string") {
				REQUIRE(p_var[name] == std::string());
				REQUIRE(p_obj[name] == std::string());
			}
		}
		WHEN("attribute 'age' was not set") {
			THEN("reading 'age' should return 0") {
				REQUIRE(p_var[age] == 0);
				REQUIRE(p_obj[age] == 0);
			}
		}
	}
}

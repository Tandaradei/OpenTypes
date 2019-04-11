#include "../src/OpenTypes.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../external/catch.hpp"

SCENARIO("types can be created", "[type]") {
	GIVEN("a new type") {
		OT_TYPE(MyType);
		REQUIRE(OT_IS_TYPE(MyType));
		REQUIRE(OT_GET_TYPE(MyType).attributes.empty());
		WHEN("an attribute is added") {
			OT_ATTR(MyType, attr, "");
			THEN("the attribute count should be 1") {
				REQUIRE(OT_GET_TYPE(MyType).attributes.size() == 1);
			}
		}

	}
}
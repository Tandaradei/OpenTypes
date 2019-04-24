#include "../src/open_types.hpp"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../external/catch.hpp"

SCENARIO("type and object creation", "[type, object]") {
	GIVEN("a new type") {
        TYPE(Person);
        WHEN("a type variable is default created") {
            Person p;
            THEN("p is nil") {
                REQUIRE(p == ot::nil);
                REQUIRE(!p);
			}
		}
        WHEN("a type variable is created with nil") {
            Person p = ot::nil;
            THEN("p is nil") {
                REQUIRE(p == ot::nil);
                REQUIRE(!p);
            }
        }
        WHEN("two type variables are created with nil") {
            Person p1 = ot::nil;
            Person p2 = ot::nil;
            THEN("p1 and p2 are equal") {
                REQUIRE(p1 == p2);
            }
        }
        WHEN("a type object is created with make_ref") {
            Person p = ot::empty();
            THEN("p is not nil") {
                REQUIRE(p != ot::nil);
                REQUIRE(p);
            }
        }
        WHEN("a type object and a nil type variable are created") {
            Person p1 = ot::empty();
            Person p2 = ot::nil;
            THEN("p1 is not equal to p2") {
                REQUIRE(p1 != p2);
            }
        }

        WHEN("two type objects are created") {
            Person p1 = ot::empty();
            Person p2 = ot::empty();
            THEN("p1 is not equal to p2") {
                REQUIRE((p1 != p2));
            }
        }
	}
}


SCENARIO("attributes", "[attribute]") {
    GIVEN("a type and an object of this type") {
        TYPE(Person);
        Person p;
        WHEN("an attribute is added") {
            //ATTR1(name, Person, std::string)
        }
    }
}

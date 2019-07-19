#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("type and object creation", "[type, object]") {
    GIVEN("a type 'Person' and a type 'Skill'") {
        WHEN("a type variable is default created") {
            Person p;
            THEN("is p nil") {
				REQUIRE(p == Person::nil());
                REQUIRE(!p);
			}
		}
        WHEN("a type variable is created with nil") {
            Person p = Person::nil();
            THEN("is p nil") {
                REQUIRE(p == Person::nil());
                REQUIRE(!p);
            }
        }
        WHEN("two type variables are created with nil") {
            Person p1 = Person::nil();
            Person p2 = Person::nil();
            THEN("are p1 and p2 equal") {
                REQUIRE(p1 == p2);
            }
        }
        WHEN("a type object is created with empty()") {
            Person p = Person::create();
            THEN("is p not nil") {
                REQUIRE(p != Person::nil());
                REQUIRE(p);
            }
        }
        WHEN("a type object and a nil type variable are created") {
            Person p1 = Person::create();
            Person p2 = Person::nil();
            THEN("is p1 not equal to p2") {
                REQUIRE(p1 != p2);
            }
        }
        WHEN("two type objects are created") {
            Person p1 = Person::create();
            Person p2 = Person::create();
            THEN("is p1 not equal to p2") {
                REQUIRE((p1 != p2));
            }
        }
		WHEN("an object and a reference to that object are created") {
			Person p1 = Person::create();
			Person p2 = p1;
			THEN("is p1 equal to p2") {
				REQUIRE((p1 == p2));
			}
		}
	}
}

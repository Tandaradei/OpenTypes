#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("type and object creation", "[type, object]") {
    GIVEN("a type 'Person' and a type 'Skill'") {
        WHEN("a type variable is default created") {
            Person p;
            THEN("is p nil") {
                REQUIRE(p == ot::nil);
                REQUIRE(!p);
			}
		}
        WHEN("a type variable is created with nil") {
            Person p = ot::nil;
            THEN("is p nil") {
                REQUIRE(p == ot::nil);
                REQUIRE(!p);
            }
        }
        WHEN("two type variables are created with nil") {
            Person p1 = ot::nil;
            Person p2 = ot::nil;
            THEN("are p1 and p2 equal") {
                REQUIRE(p1 == p2);
            }
        }
        WHEN("a type object is created with empty()") {
            Person p = ot::empty();
            THEN("is p not nil") {
                REQUIRE(p != ot::nil);
                REQUIRE(p);
            }
        }
		WHEN("a type object is created with constructor") {
			Person p(true);
			THEN("is p not nil") {
				REQUIRE(p != ot::nil);
				REQUIRE(p);
			}
		}
        WHEN("a type object and a nil type variable are created") {
            Person p1 = ot::empty();
            Person p2 = ot::nil;
            THEN("is p1 not equal to p2") {
                REQUIRE(p1 != p2);
            }
        }
        WHEN("two type objects are created") {
            Person p1 = ot::empty();
            Person p2 = ot::empty();
            THEN("is p1 not equal to p2") {
                REQUIRE((p1 != p2));
            }
        }
		WHEN("two type objects of different types are created") {
			Person p = ot::empty();
			Skill s = ot::empty();
			THEN("is p not equal to s") {
				REQUIRE((p != s));
			}
		}
		WHEN("an object and a reference to that object are created") {
			Person p1 = ot::empty();
			Person p2 = p1;
			THEN("is p1 equal to p2") {
				REQUIRE((p1 == p2));
			}
		}
	}
}

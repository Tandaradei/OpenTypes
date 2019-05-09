#include "external/catch.hpp"

#include "test/Types/Person.hpp"

SCENARIO("type and object creation", "[type, object]") {
    GIVEN("a type 'Person'") {
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
        WHEN("a type object is created with empty()") {
            Person p = ot::empty();
            THEN("p is not nil") {
                REQUIRE(p != ot::nil);
                REQUIRE(p);
            }
        }
		WHEN("a type object is created with constructor") {
			Person p(true);
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
		WHEN("two type objects of different types are created") {
			Person p = ot::empty();
			Skill s = ot::empty();
			THEN("p is not equal to s") {
				REQUIRE((p != s));
			}
		}
	}
}

#if SEPERATE_TEST_FILES == 0

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

#endif // SEPERATE_TEST_FILES
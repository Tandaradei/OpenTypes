#include "external/catch.hpp"

#include "test/Types/Car.hpp"

SCENARIO("relations between types", "[type, relation]") {
    GIVEN("an object of type Car and an object of type Engine") {
        Car car_obj = ot::empty();
        Engine engine_obj = ot::empty();
        THEN("there should be no connection between them") {
            REQUIRE((car_obj[engine] == ot::nil));
            REQUIRE((engine_obj[car] == ot::nil));
        }
        GIVEN("engine_obj set as engine for car_obj") {
            car_obj(engine, engine_obj);
            THEN("attribute engine of car_obj should be equal to engine_obj") {
                REQUIRE((car_obj[engine] == engine_obj));
            }
            THEN("attribute car of engine_obj should be equal to car_obj") {
                REQUIRE((engine_obj[car] == car_obj));
            }
            GIVEN("engine_obj removed as engine for car_obj") {
                car_obj -= engine;
                THEN("attribute engine of car_obj should be nil") {
                    REQUIRE((car_obj[engine] == ot::nil));
                }
                THEN("attribute car of engine_obj should be nil") {
                    REQUIRE((engine_obj[car] == ot::nil));
                }
            }
        }
        GIVEN("car_obj set as car for engine_obj") {
            engine_obj(car, car_obj);
            THEN("attribute engine of car_obj should be equal to engine_obj") {
                REQUIRE((car_obj[engine] == engine_obj));
            }
            THEN("attribute car of engine_obj should be equal to car_obj") {
                REQUIRE((engine_obj[car] == car_obj));
            }
            GIVEN("engine_obj removed as engine for car_obj") {
                car_obj -= engine;
                THEN("attribute engine of car_obj should be nil") {
                    REQUIRE((car_obj[engine] == ot::nil));
                }
                THEN("attribute car of engine_obj should be nil") {
                    REQUIRE((engine_obj[car] == ot::nil));
                }
            }
        }
    }
    GIVEN("an object of type Car and two objects of type Engine") {
        Car car_obj = ot::empty();
        Engine engine_obj1 = ot::empty();
        Engine engine_obj2 = ot::empty();
        GIVEN("engine_obj1 set as engine for car_obj") {
            car_obj(engine, engine_obj1);
            THEN("engine of car_obj should be equal to engine_obj1 and not equal to engine_obj2") {
                REQUIRE((car_obj[engine] == engine_obj1));
                REQUIRE((car_obj[engine] != engine_obj2));
            }
            THEN("car of engine_obj1 should be equal to car_obj") {
                REQUIRE((engine_obj1[car] == car_obj));
            }
            THEN("car of engine_obj1 should be nil") {
                REQUIRE((engine_obj2[car] == ot::nil));
            }
            GIVEN("engine_obj2 set as engine for car_obj") {
                car_obj(engine, engine_obj2);
                THEN("engine of car_obj should be not equal to engine_obj1 and equal to engine_obj2") {
                    REQUIRE((car_obj[engine] != engine_obj1));
                    REQUIRE((car_obj[engine] == engine_obj2));
                }
                THEN("car of engine_obj1 should be nil") {
                    REQUIRE((engine_obj1[car] == ot::nil));
                }
                THEN("car of engine_obj1 should be equal to car_obj") {
                    REQUIRE((engine_obj2[car] == car_obj));
                }
            }
        }
    }
}

/*-------------------------------*\
Copyright 2019 Kiyo Matsui
<<<<<<< HEAD
KiyosGameEngine v0.73
=======
KiyosGameEngine v0.74
>>>>>>> kiyodev
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

// test typeTrigger

#include "catch2/catch.hpp"

#define private public
#define protected public

#include "kgeECS.h"

class testTypeTrigger {
public:
  testTypeTrigger() {}
  ~testTypeTrigger() {}
};

class testTypeTriggerNull {
public:
  testTypeTriggerNull() {}
  ~testTypeTriggerNull() {}
};

enum class Color { red, green, blue };

TEST_CASE("simple tests") {
  kge::typeTrigger<testTypeTrigger> a;

  SECTION("first test") {
    REQUIRE(a.button == false);
    a.trigger();
    REQUIRE(a.check() == true);
    REQUIRE(a.button == true);
    REQUIRE(a.reset() == true);
    REQUIRE(a.button == false);
    a.trigger();
  }
  SECTION("Test new trigger of same type") {
    kge::typeTrigger<testTypeTrigger> b;
    REQUIRE(b.check() == true);
    REQUIRE(b.button == true);
    REQUIRE(b.reset() == true);
    REQUIRE(b.button == false);
    a.trigger();
  }

  SECTION("Test new trigger of another type") {
    kge::typeTrigger<testTypeTriggerNull> c;
    REQUIRE(c.check() == false);
    REQUIRE(c.button == false);
    REQUIRE(c.reset() == false);
    REQUIRE(c.button == false);
  }
}

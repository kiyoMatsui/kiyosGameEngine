/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.7
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

// test point

#include "catch2/catch.hpp"

#include <iostream>
#define USED_FOR_KGE_TESTS

#define private public
#define protected public

#include "kgePoint.h"

TEST_CASE("build test") {
  kgeTestPrint("test: call each constructor twice:");
  kge::point<int> a(1, 2);
  kge::point<float> b;
  kge::point<double> c(1.1, 2.1);
  kge::point<unsigned int> d;
  kgeTestPrint("end test int:");
}

TEST_CASE("test int assignment = ") {
  kgeTestPrint("test int: assignment = ");
  kge::point<int> a(11, 22);
  kge::point<int> b;
  b = a;
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int +assignment += ") {
  kgeTestPrint("test int: assignment += ");
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  b += a;
  REQUIRE(b.x == 21);
  REQUIRE(b.y == 42);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int -assignment -= ") {
  kgeTestPrint("test int: assignment -= ");
  kge::point<int> a(11, 22);
  kge::point<int> b(30, 30);
  b -= a;
  REQUIRE(b.x == 19);
  REQUIRE(b.y == 8);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int + operator") {
  kgeTestPrint("test int: + operator");
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  kge::point<int> c = b + a;
  REQUIRE(c.x == 21);
  REQUIRE(c.y == 42);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int - operator") {
  kgeTestPrint("test int: - operator");
  kge::point<int> a(11, 22);
  kge::point<int> b(20, 30);
  kge::point<int> c = b - a;
  REQUIRE(c.x == 9);
  REQUIRE(c.y == 8);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int == & >= & <= operator") {
  kgeTestPrint("test int: == & >= & <= operator");
  kge::point<int> a(11, 22);
  kge::point<int> b(11, 22);
  REQUIRE(a == b);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int != operator") {
  kgeTestPrint("test int: != operator");
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  kge::point<int> c(12, 22);
  REQUIRE(a != b);
  REQUIRE(a != c);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int > & >= operator") {
  kgeTestPrint("test int: > & >= operator");
  kge::point<int> a(12, 23);
  kge::point<int> b(11, 22);
  REQUIRE(a > b);
  REQUIRE(a >= b);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int < & <= operator") {
  kgeTestPrint("test int: < & <= operator");
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  REQUIRE(a < b);
  REQUIRE(a <= b);
  kgeTestPrint("end test int:");
}

TEST_CASE("test int copy constructor") {
  kgeTestPrint("test int: copy constructor");
  kge::point<int> a(11, 22);
  kge::point<int> b(a);
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
  kgeTestPrint("end test int:");
}

TEST_CASE("test double != and == operator") {
  kgeTestPrint("test int: != operator");
  kge::point<double> a(10.0001, 22.0001);
  kge::point<double> b(10.0002, 21.99999);
  kge::point<double> c(10.0102, 21.99999);
  REQUIRE(a == b);
  REQUIRE(c != b);
  REQUIRE(c != a);
  kgeTestPrint("end test double:");
}

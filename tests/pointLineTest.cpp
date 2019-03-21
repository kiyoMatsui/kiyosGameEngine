/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.74
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

#include "kgePointLine.h"

TEST_CASE("build test") {
  kge::point<int> a(1, 2);
  kge::point<float> b;
  kge::point<double> c(1.1, 2.1);
  kge::point<unsigned int> d;
  kge::point<double> e(c);
  REQUIRE(e.x == 1.1);
  REQUIRE(e.y == 2.1);
}

TEST_CASE("test int assignment = ") {
  kge::point<int> a(11, 22);
  kge::point<int> b;
  b = a;
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
}

TEST_CASE("test int += ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  a += 7;
  b += a;
  REQUIRE(b.x == 28);
  REQUIRE(b.y == 49);
  REQUIRE(a.x == 18);
  REQUIRE(a.y == 29);
}

TEST_CASE("test int -= ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(30, 30);
  b -= a;
  REQUIRE(b.x == 19);
  REQUIRE(b.y == 8);
  b-= 2;
  REQUIRE(b.x == 17);
  REQUIRE(b.y == 6);
}

TEST_CASE("test int *= ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  b *= a;
  REQUIRE(b.x == 22);
  REQUIRE(b.y == 66);
  b *= 2;
  REQUIRE(b.x == 44);
  REQUIRE(b.y == 132);
}

TEST_CASE("test int /= ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  b *= a;
  REQUIRE(b.x == 22);
  REQUIRE(b.y == 66);
  b *= 2;
  REQUIRE(b.x == 44);
  REQUIRE(b.y == 132);
}

TEST_CASE("test int length ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  REQUIRE(b.length(a) == 21);
}

TEST_CASE("test int + operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  kge::point<int> c = b + a;
  REQUIRE(c.x == 21);
  REQUIRE(c.y == 42);
  c = b + 1;
  REQUIRE(c.x == 11);
  REQUIRE(c.y == 21);
}

TEST_CASE("test int - operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(20, 30);
  kge::point<int> c = b - a;
  REQUIRE(c.x == 9);
  REQUIRE(c.y == 8);
  c = c - 3;
  REQUIRE(c.x == 6);
  REQUIRE(c.y == 5);
}

TEST_CASE("test int * operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  kge::point<int> c = b * a;
  REQUIRE(c.x == 22);
  REQUIRE(c.y == 66);
  a = b * 10;
  REQUIRE(a.x == 20);
  REQUIRE(a.y == 30);
}

TEST_CASE("test int / operator") {
  kge::point<int> a(2, 6);
  kge::point<int> b(20, 30);
  kge::point<int> c = b / a;
  REQUIRE(c.x == 10);
  REQUIRE(c.y == 5);
  a = b / 2;
  REQUIRE(a.x == 10);
  REQUIRE(a.y == 15);  
}

TEST_CASE("test int == & >= & <= operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(11, 22);
  REQUIRE(a == b);
}

TEST_CASE("test int != operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  kge::point<int> c(12, 22);
  REQUIRE(a != b);
  REQUIRE(a != c);
}

TEST_CASE("test int > & >= operator") {
  kge::point<int> a(12, 23);
  kge::point<int> b(11, 22);
  REQUIRE(a > b);
  REQUIRE(a >= b);
}

TEST_CASE("test int < & <= operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  REQUIRE(a < b);
  REQUIRE(a <= b);
}

TEST_CASE("test int copy constructor") {
  kge::point<int> a(11, 22);
  kge::point<int> b(a);
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
}

TEST_CASE("test double != and == operator") {
  kge::point<double> a(10.0001, 22.0001);
  kge::point<double> b(10.0002, 21.99999);
  kge::point<double> c(10.0102, 21.99999);
  REQUIRE(a == b);
  REQUIRE(c != b);
  REQUIRE(c != a);
}

TEST_CASE("line build test") {
  kge::point<double> a(2, 2);
  kge::point<double> b(12, 22);
  kge::line<double> c(a, b);
  kge::line<double> d(c);
  REQUIRE(d.A == c.A);
  REQUIRE(d.B == c.B);
  REQUIRE(b == c.B);
}

TEST_CASE("test line basic") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(1, 11);
  kge::point<double> e(11, 1);
  kge::line<double> f(d, e);
  REQUIRE(c.length() <  14.15);
  REQUIRE(c.length() >  14.13);
  kge::line<double> g = c + f;
  REQUIRE(g.A ==  c.A);
  REQUIRE(g.B.x == 1.0 );
  REQUIRE(g.B.y == 21.0 );
  REQUIRE(c.intersect(f) == true);
  REQUIRE(c.recentIntersection.x == 6.0);
  REQUIRE(c.recentIntersection.y == 6.0);
}

TEST_CASE("test line not intersecting") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(1, 11);
  kge::point<double> e(2, 10);
  kge::line<double> f(d, e);
  REQUIRE(c.length() <  14.15);
  REQUIRE(c.length() >  14.13);
  kge::line<double> g = c + f;
  REQUIRE(g.A ==  c.A);
  REQUIRE(g.B.x == 10.0 );
  REQUIRE(g.B.y == 12.0 );
  REQUIRE(c.intersect(f) == false);
}

TEST_CASE("test line parallel") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(2, 2);
  kge::point<double> e(12, 12);
  kge::line<double> f(d, e);
  REQUIRE(c.intersect(f) == false);
}

TEST_CASE("test line basic2") {
  kge::point<double> a(11, 11);
  kge::point<double> b(1, 1);
  kge::line<double> c(a, b);
  kge::point<double> d(11, 1);
  kge::point<double> e(1, 11);
  kge::line<double> f(d, e);
  REQUIRE(c.length() <  14.15);
  REQUIRE(c.length() >  14.13);
  REQUIRE(c.intersect(f) == true);
  REQUIRE(c.recentIntersection.x == 6.0);
  REQUIRE(c.recentIntersection.y == 6.0);
}

/*-------------------------------*\
Copyright 2020 Kiyo Matsui
KiyosGameEngine v1.0 
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#include "catch2/catch.hpp"

#include <iostream>
#define USED_FOR_KGE_TESTS

#define private public
#define protected public

#include "kgePointLine.h"

TEST_CASE("point test constructor and copy/move constructor test") {
  kge::point<int> a(1, 2);
  kge::point<float> b;
  kge::point<unsigned int> c(1, 2);
  kge::point<unsigned int> d(c);
  kge::point<unsigned int> e(c);
  REQUIRE(e.x == 1);
  REQUIRE(e.y == 2);
  kge::point<unsigned int> f(std::move(e));
  REQUIRE(f.x == 1);
  REQUIRE(f.y == 2);
}

TEST_CASE("point test copy/move assignment = ") {
  kge::point<int> a(11, 22);
  kge::point<int> b;
  b = a;
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
  kge::point<int> c;
  c = std::move(b);
  REQUIRE(b.x == 11);
  REQUIRE(b.y == 22);
}

TEST_CASE("point test int += ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  a += 7;
  b += a;
  REQUIRE(b.x == 28);
  REQUIRE(b.y == 49);
  REQUIRE(a.x == 18);
  REQUIRE(a.y == 29);
}

TEST_CASE("point test int -= ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(30, 30);
  b -= a;
  REQUIRE(b.x == 19);
  REQUIRE(b.y == 8);
  b -= 2;
  REQUIRE(b.x == 17);
  REQUIRE(b.y == 6);
}

TEST_CASE("point test int *= ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  b *= a;
  REQUIRE(b.x == 22);
  REQUIRE(b.y == 66);
  b *= 2;
  REQUIRE(b.x == 44);
  REQUIRE(b.y == 132);
}

TEST_CASE("point test int /= ") {
  kge::point<int> a(2, 3);
  kge::point<int> b(24, 27);
  b /= a;
  REQUIRE(b.x == 12);
  REQUIRE(b.y == 9);
  b /= 3;
  REQUIRE(b.x == 4);
  REQUIRE(b.y == 3);
}

TEST_CASE("point test int length ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  REQUIRE(b.length(a) == 21);
  REQUIRE(a.length(b) == 21);
  kge::point<int> c(9, 99);
  kge::point<int> d(-1, 99);
  REQUIRE(c.length(d) == 10);
  REQUIRE(d.length(c) == 10);
}

TEST_CASE("point test int + operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(10, 20);
  kge::point<int> c = b + a;
  REQUIRE(c.x == 21);
  REQUIRE(c.y == 42);
  c = b + 1;
  REQUIRE(c.x == 11);
  REQUIRE(c.y == 21);
}

TEST_CASE("point test int - operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(20, 30);
  kge::point<int> c = b - a;
  REQUIRE(c.x == 9);
  REQUIRE(c.y == 8);
  c = c - 3;
  REQUIRE(c.x == 6);
  REQUIRE(c.y == 5);
}

TEST_CASE("point test int * operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(2, 3);
  kge::point<int> c = b * a;
  REQUIRE(c.x == 22);
  REQUIRE(c.y == 66);
  a = b * 10;
  REQUIRE(a.x == 20);
  REQUIRE(a.y == 30);
}

TEST_CASE("point test int / operator") {
  kge::point<int> a(2, 6);
  kge::point<int> b(20, 30);
  kge::point<int> c = b / a;
  REQUIRE(c.x == 10);
  REQUIRE(c.y == 5);
  a = b / 2;
  REQUIRE(a.x == 10);
  REQUIRE(a.y == 15);
}

TEST_CASE("point test int == ") {
  kge::point<int> a(11, 22);
  kge::point<int> b(11, 22);
  REQUIRE(a == b);
}

TEST_CASE("point test int != operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  kge::point<int> c(12, 22);
  REQUIRE(a != b);
  REQUIRE(a != c);
}

TEST_CASE("point test int > & >= operator") {
  kge::point<int> a(12, 23);
  kge::point<int> b(11, 22);
  REQUIRE(a > b);
  REQUIRE(a >= b);
}

TEST_CASE("point test int < & <= operator") {
  kge::point<int> a(11, 22);
  kge::point<int> b(12, 23);
  REQUIRE(a < b);
  REQUIRE(a <= b);
}

TEST_CASE("test double != and == operator") {
  kge::point<double> a(10.0001, 22.0001);
  kge::point<double> b(10.0002, 21.99999);
  kge::point<double> c(10.0102, 21.99999);
  REQUIRE(a == b);
  REQUIRE(c != b);
  REQUIRE(c != a);
}

TEST_CASE("line test constructor and copy/move constructor and assignment") {
  kge::point<double> a(2, 2);
  kge::point<double> b(12, 22);
  kge::line<double> c(a, b);
  kge::line<double> d(c);
  REQUIRE(d.A == c.A);
  REQUIRE(d.B == c.B);
  REQUIRE(b == c.B);
  kge::line<double> e(std::move(d));
  REQUIRE(e.A == c.A);
  REQUIRE(e.B == c.B);
  kge::line<double> f = d;
  kge::line<double> g = std::move(d);
  REQUIRE(f.A == c.A);
  REQUIRE(f.B == c.B);
  REQUIRE(g.A == c.A);
  REQUIRE(g.B == c.B);
}

TEST_CASE("line method tests") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(1, 11);
  kge::point<double> e(11, 1);
  kge::line<double> f(d, e);
  REQUIRE(c.length() < 14.15);
  REQUIRE(c.length() > 14.13);
  kge::line<double> g = c + f;
  REQUIRE(g.A == c.A);
  REQUIRE(g.B.x == 1.0);
  REQUIRE(g.B.y == 21.0);
  REQUIRE(c.intersect(f) == true);
  REQUIRE(c.recentIntersection.x == 6.0);
  REQUIRE(c.recentIntersection.y == 6.0);
}

TEST_CASE("line not intersecting tests") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(1, 11);
  kge::point<double> e(2, 10);
  kge::line<double> f(d, e);
  REQUIRE(c.length() < 14.15);
  REQUIRE(c.length() > 14.13);
  kge::line<double> g = c + f;
  REQUIRE(g.A == c.A);
  REQUIRE(g.B.x == 10.0);
  REQUIRE(g.B.y == 12.0);
  REQUIRE(c.intersect(f) == false);
}

TEST_CASE("line parallel tests") {
  kge::point<double> a(1, 1);
  kge::point<double> b(11, 11);
  kge::line<double> c(a, b);
  kge::point<double> d(2, 2);
  kge::point<double> e(12, 12);
  kge::line<double> f(d, e);
  REQUIRE(c.intersect(f) == false);
}

TEST_CASE("line extra tests") {
  kge::point<double> a(11, 11);
  kge::point<double> b(1, 1);
  kge::line<double> c(a, b);
  kge::point<double> d(11, 1);
  kge::point<double> e(1, 11);
  kge::line<double> f(d, e);
  REQUIRE(c.length() < 14.15);
  REQUIRE(c.length() > 14.13);
  REQUIRE(c.intersect(f) == true);
  REQUIRE(c.recentIntersection.x == 6.0);
  REQUIRE(c.recentIntersection.y == 6.0);
}


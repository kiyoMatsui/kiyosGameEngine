/*-------------------------------*\
Copyright 2021 Kiyo Matsui
KiyosGameEngine v1.3
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#include "catch2/catch.hpp"

#include <iostream>
#include <string>
#define USED_FOR_KGE_TESTS

#define private public
#define protected public

#include "kgeECS.h"

struct testComponent {
  int mass{5};
  int speed{10};
};

TEST_CASE("test entity constructor and copy/move constructors and copy/move assignment ") {
  kge::entity a(true, 7, 1);
  kge::entity b(false, 8, 12);
  b.set(a);
  REQUIRE(b.alive == true);
  REQUIRE(b.ID == 8);
  REQUIRE(b.type == 1);
  kge::entity c(a);
  REQUIRE(c.alive == true);
  REQUIRE(c.ID == 7);
  REQUIRE(c.type == 1);

  kge::entity d(std::move(b));
  REQUIRE(d.alive == true);
  REQUIRE(d.ID == 8);
  REQUIRE(d.type == 1);
}

TEST_CASE("test entityHandler and componentHandler") {
  kge::entityHandler eh;
  kge::componentHandler<testComponent> ch(eh);

  SECTION("entityHandler tests") {
    REQUIRE(eh.containerSize == 10);
    REQUIRE(eh.container.size() == 10);
    REQUIRE(eh.deadStack.size() == 10);
    REQUIRE(eh.deadStack[0] == 0);
    REQUIRE(eh.deadStack[4] == 4);
    REQUIRE_FALSE(eh.dummyEntity.alive);
    REQUIRE(eh.getItem(3).ID == 3);
    REQUIRE(eh.build(9).type == 9);
    REQUIRE(eh.build(9).ID == 8);
    REQUIRE(eh.build(9).ID == 7);
    REQUIRE(eh.build(9).ID == 6);
    REQUIRE(eh.build(9).ID == 5);
    REQUIRE(eh.build(9).ID == 4);
    REQUIRE(eh.build(9).ID == 3);
    REQUIRE(eh.build(9).ID == 2);
    REQUIRE(eh.build(9).ID == 1);
    REQUIRE(eh.build(9).ID == 0);
    REQUIRE(eh.build(9).ID == 999999);
    REQUIRE(eh.getItem(4).alive);
    eh.destroy(4);
    REQUIRE_FALSE(eh.getItem(4).alive);
    REQUIRE(eh.deadStack.size() == 1);
    eh.for_each([&](auto& ent) { REQUIRE(eh.getItem(ent.ID).alive); });
    eh.build(42);
    int counter = 0;
    eh.for_each_type(42, [&](auto& ent) {
      REQUIRE(eh.getItem(ent.ID).type == 42);
      counter++;
    });
    REQUIRE(counter == 1);
  }

  SECTION("componentHandler tests") {
    REQUIRE(ch.containerSize == 10);
    REQUIRE(ch.container.size() == 10);
    REQUIRE(ch.dummyComponent.mass == 5);
    REQUIRE(ch.getItem(eh.getItem(2).ID).speed == 10);
    REQUIRE(ch.getItem(eh.getItem(2)).speed == 10);
    std::vector<testComponent> temp = ch.getData();
    for (auto& tempItem : temp) {
      REQUIRE(tempItem.speed == 10);
    }
  }
}

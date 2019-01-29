/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.7
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

// test baseEntity

#include "catch2/catch.hpp"

#define private public
#define protected public

#include "kgeECS.h"

class testBaseEntity1 : public kge::baseEntity {
public:
  testBaseEntity1() {}
  ~testBaseEntity1() {}
};

class testBaseEntity2 : public kge::baseEntity {
public:
  testBaseEntity2() {}
  ~testBaseEntity2() {}
};

TEST_CASE("simple build") {
  auto a = std::make_shared<testBaseEntity1>();
  auto b = std::make_shared<testBaseEntity1>();
  auto c = std::make_shared<testBaseEntity2>();
  auto d = std::make_shared<testBaseEntity1>();

  SECTION("delete and recreate") {
    REQUIRE(a->ID == 0);
    REQUIRE(b->ID == 1);
    REQUIRE(c->ID == 2);
    REQUIRE(d->ID == 3);
    b.reset();
    kge::baseEntity::recycleIDs();
    auto e = std::make_shared<testBaseEntity2>();
    INFO("new entities should take deleted ID");
    REQUIRE(e->ID == 1);
    a.reset();
    d.reset();
    kge::baseEntity::recycleIDs();

    auto f = std::make_shared<testBaseEntity1>();
    auto g = std::make_shared<testBaseEntity2>();
    auto h = std::make_shared<testBaseEntity1>();

    REQUIRE(f->ID == 0);
    REQUIRE(g->ID == 3);
    REQUIRE(h->ID == 4);
    REQUIRE(a->newID_AndNoOfMaxEntities == 5);
    h.reset();
    f.reset();
    g.reset();
    kge::baseEntity::recycleIDs();
    INFO("checking ID bin");
    REQUIRE(a->idBin[0] == 3);
    REQUIRE(a->idBin[1] == 0);
    REQUIRE(a->idBin[2] == 4);
  }
}
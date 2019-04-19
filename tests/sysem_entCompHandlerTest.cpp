/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.8
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

// test systems entCompHandler

#include "catch2/catch.hpp"

#include <iostream>
#include <string>
#define USED_FOR_KGE_TESTS

#define private public
#define protected public

#include "kgeECS.h"

class poundEntity : public kge::baseEntity {
public:
  poundEntity(int a) : exampleData(a) {}

  int exampleData;
};

class dollarEntity : public kge::baseEntity {
public:
  dollarEntity(int a) : exampleData(a) {}

  int exampleData;
};

class position {
public:
  position(int a) : pos(a) {}
  int pos = 0;
  int exampleData = 0;
};

class renderChar {
public:
  renderChar(std::string a) : character(a) {}
  std::string character;
  int exampleData = 0;
};

class renderSystem : public kge::system {
public:
  renderSystem(kge::entCompHandler<kge::baseEntity> &aEntities,
               kge::entCompHandler<renderChar> &aChar)
      : mEntities(aEntities), mChar(aChar) {}

  int update(double dt) { return 1; }

private:
  kge::entCompHandler<kge::baseEntity> &mEntities;
  kge::entCompHandler<renderChar> &mChar;
};

class movementSystem : public kge::system {
public:
  movementSystem(kge::entCompHandler<kge::baseEntity> &aEntities,
                 kge::entCompHandler<position> &aPosition)
      : mEntities(aEntities), mPosition(aPosition) {}

  int update(double dt) { return 1; }

private:
  kge::entCompHandler<kge::baseEntity> &mEntities;
  kge::entCompHandler<position> &mPosition;
};

TEST_CASE("simple build") {
  kge::entCompHandler<kge::baseEntity> mEntities;
  kge::entCompHandler<position> mPosition;
  kge::entCompHandler<renderChar> mChar;
  movementSystem mMoveSystem(mEntities, mPosition);
  renderSystem mRenderSystem(mEntities, mChar);
  kge::cleanUpSystem<kge::entCompHandler<kge::baseEntity>,
                     kge::entCompHandler<position>,
                     kge::entCompHandler<renderChar>>
      mCleanupSystem(mEntities, mPosition, mChar);
  REQUIRE(mEntities.container.size() == 0);
  REQUIRE(mPosition.container.size() == 0);
  REQUIRE(mChar.container.size() == 0);
  REQUIRE(std::get<0>(mCleanupSystem.componentsTuple).container.size() == 0);
  REQUIRE(std::get<1>(mCleanupSystem.componentsTuple).container.size() == 0);
  REQUIRE(std::get<2>(mCleanupSystem.componentsTuple).container.size() == 0);
  REQUIRE(kge::baseEntity::newID_AndNoOfMaxEntities == 0);
  REQUIRE(kge::baseEntity::freshBin.size() == 0);
  REQUIRE(kge::baseEntity::idBin.size() == 0);

  // first entity

  mEntities.getItem(0) = std::make_unique<poundEntity>(42);
  REQUIRE(kge::baseEntity::newID_AndNoOfMaxEntities == 1);
  REQUIRE(mEntities.container.size() == 1);
  REQUIRE(mPosition.container.size() == 0);
  REQUIRE(mChar.container.size() == 0);
  mPosition.getItem(0) = std::make_unique<position>(0);
  mChar.getItem(0) = std::make_unique<renderChar>("£");
  REQUIRE(mPosition.container.size() == 1);
  REQUIRE(mChar.container.size() == 1);

  // second entity

  mEntities.getItem(1) = std::make_unique<poundEntity>(42);
  mPosition.getItem(1) = std::make_unique<position>(0);
  mChar.getItem(1) = std::make_unique<renderChar>("£");

  mEntities.getItem(2) = std::make_unique<poundEntity>(42);
  mPosition.getItem(2) = std::make_unique<position>(0);
  mChar.getItem(2) = std::make_unique<renderChar>("£");

  REQUIRE(mPosition.container.size() == 3);
  REQUIRE(mChar.container.size() == 3);

  mEntities.getItem(3) = std::make_unique<dollarEntity>(42);
  // mPosition.getItem(3) = std::make_unique<position>(0);
  mChar.getItem(3) = std::make_unique<renderChar>("$");

  REQUIRE(mPosition.container.size() == 3);
  REQUIRE(mChar.container.size() == 4);

  mEntities.getItem(4) = std::make_unique<dollarEntity>(42);
  mPosition.getItem(4) = std::make_unique<position>(0);
  mChar.getItem(4) = std::make_unique<renderChar>("$");

  REQUIRE(mEntities.container.size() == 5);
  REQUIRE(mPosition.container.size() == 5);
  REQUIRE(mChar.container.size() == 5);
  REQUIRE(kge::baseEntity::newID_AndNoOfMaxEntities == 5);
  REQUIRE(kge::baseEntity::getNextID() == 5); // 6th ID

  REQUIRE(mPosition.getItem(0) != nullptr);
  REQUIRE(mPosition.getItem(1) != nullptr);
  REQUIRE(mPosition.getItem(2) != nullptr);
  REQUIRE(mPosition.getItem(3) == nullptr);
  REQUIRE(mPosition.getItem(4) != nullptr);

  REQUIRE(mEntities.getItem(0) != nullptr);
  REQUIRE(mEntities.getItem(1) != nullptr);
  REQUIRE(mEntities.getItem(2) != nullptr);
  REQUIRE(mEntities.getItem(3) != nullptr);
  REQUIRE(mEntities.getItem(4) != nullptr);

  mEntities.getItem(2).reset(nullptr);
  mEntities.getItem(3).reset(nullptr);

  REQUIRE(kge::baseEntity::freshBin.size() == 2);
  REQUIRE(kge::baseEntity::freshBin.back() == 3);
  REQUIRE(kge::baseEntity::freshBin.rbegin()[1] == 2);

  int temp1 = 0;
  for (auto &i : mEntities.container) {
    if (i.get()) {
      if (mPosition.getItem(i->ID) && mChar.getItem(i->ID)) {
        REQUIRE(i->ID == temp1);
        temp1++;
        if (temp1 == 2)
          temp1 = 4;
      }
    }
  }
  REQUIRE(mPosition.getItem(0) != nullptr);
  REQUIRE(mPosition.getItem(1) != nullptr);
  REQUIRE(mPosition.getItem(2) != nullptr);
  REQUIRE(mPosition.getItem(3) == nullptr);
  REQUIRE(mPosition.getItem(4) != nullptr);

  REQUIRE(mChar.getItem(0) != nullptr);
  REQUIRE(mChar.getItem(1) != nullptr);
  REQUIRE(mChar.getItem(2) != nullptr);
  REQUIRE(mChar.getItem(3) != nullptr);
  REQUIRE(mChar.getItem(4) != nullptr);

  REQUIRE(kge::baseEntity::freshBin.size() == 2);
  REQUIRE(kge::baseEntity::idBin.size() == 0);

  mCleanupSystem.update(1.1);

  REQUIRE(kge::baseEntity::freshBin.size() == 0);
  REQUIRE(kge::baseEntity::idBin.size() == 2);

  REQUIRE(kge::baseEntity::idBin.back() == 2);
  REQUIRE(kge::baseEntity::idBin.rbegin()[1] == 3);

  REQUIRE(mPosition.getItem(0) != nullptr);
  REQUIRE(mPosition.getItem(1) != nullptr);
  REQUIRE(mPosition.getItem(2) == nullptr);
  REQUIRE(mPosition.getItem(3) == nullptr);
  REQUIRE(mPosition.getItem(4) != nullptr);

  REQUIRE(mChar.getItem(0) != nullptr);
  REQUIRE(mChar.getItem(1) != nullptr);
  REQUIRE(mChar.getItem(2) == nullptr);
  REQUIRE(mChar.getItem(3) == nullptr);
  REQUIRE(mChar.getItem(4) != nullptr);

  REQUIRE(mEntities.getItem(0) != nullptr);
  REQUIRE(mEntities.getItem(1) != nullptr);
  REQUIRE(mEntities.getItem(2) == nullptr);
  REQUIRE(mEntities.getItem(3) == nullptr);
  REQUIRE(mEntities.getItem(4) != nullptr);

  REQUIRE(kge::baseEntity::getNextID() == 2);

  mEntities.getItem(kge::baseEntity::getNextID()) =
      std::make_unique<poundEntity>(42);
  REQUIRE(kge::baseEntity::idBin.back() == 3);
  REQUIRE(kge::baseEntity::idBin.back() == kge::baseEntity::getNextID());
  REQUIRE(kge::baseEntity::getNextID() == 3);
  REQUIRE(mEntities.container.size() == 5);
  unsigned int a =
      kge::baseEntity::getNextID(); // very interesitng this only works like
                                    // this... otherwise segfault
  mEntities.getItem(a) = std::make_unique<poundEntity>(42);
  a = kge::baseEntity::getNextID();
  mEntities.getItem(a) = std::make_unique<poundEntity>(42);
  REQUIRE(mEntities.container.size() == 6);
  REQUIRE(kge::baseEntity::getNextID() == 6);
}

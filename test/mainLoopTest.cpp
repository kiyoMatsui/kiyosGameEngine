#include "catch2/catch.hpp"

#include <iostream>
#define USED_FOR_KGE_TESTS

#ifdef USED_FOR_KGE_TESTS
#define kgeTestPrint(x) std::cout << x << std::endl
#else
#define kgeTestPrint(x)
#endif

#define private public
#define protected public

#include "kgeMainLoop.h"
#include "kgePointLine.h"

class testState1;
class testState2;
class testState3;
class testState4;

class testData {
  int x = 0;
  int y = 1;
  int z = 2;
};

class testState1 : public kge::abstractState {
 public:
  testState1(kge::mainLoop* mainLoopRef) : mMainLoopPtr(mainLoopRef) { kgeTestPrint("constructor testState1"); }
  ~testState1() { kgeTestPrint("destructor testState1"); }
  void update(double a) {
    a++;
    INFO("checking correct state is sent");
    REQUIRE(dynamic_cast<testState1*>(mMainLoopPtr->peekState())->counter == 1);
    INFO("checking correct stateStack size");
    REQUIRE(mMainLoopPtr->stateStack.size() == 1);
    REQUIRE(mMainLoopPtr->changeStatePtr == nullptr);
    mMainLoopPtr->popState();
    REQUIRE(mMainLoopPtr->changeStatePtr != nullptr);
  }
  void processEvents() {}
  void render(double a) { a++; }
  kge::mainLoop* mMainLoopPtr;
  int counter = 1;
};

class testState2 : public kge::abstractState {
 public:
  testState2(kge::mainLoop* mainLoopRef) : mMainLoopPtr(mainLoopRef) { kgeTestPrint("constructor testState2"); }
  ~testState2() { kgeTestPrint("destructor testState2"); }
  void update(double a) {
    a++;
    INFO("checking correct state is sent");
    REQUIRE(dynamic_cast<testState2*>(mMainLoopPtr->peekState())->counter == 2);
    INFO("checking correct stateStack size");
    REQUIRE(mMainLoopPtr->stateStack.size() == 1);
    REQUIRE(mMainLoopPtr->changeStatePtr == nullptr);
    mMainLoopPtr->switchState<testState3>(mMainLoopPtr);
    REQUIRE(mMainLoopPtr->changeStatePtr != nullptr);
  }
  void processEvents() {}
  void render(double a) { a++; }
  kge::mainLoop* mMainLoopPtr;
  int counter = 2;
};

class testState3 : public kge::abstractState {
 public:
  testState3(kge::mainLoop* mainLoopRef) : mMainLoopPtr(mainLoopRef) { kgeTestPrint("constructor testState3"); }
  ~testState3() { kgeTestPrint("destructor testState3"); }
  void update(double a) {
    a++;
    INFO("checking correct state is sent");
    REQUIRE(dynamic_cast<testState3*>(mMainLoopPtr->peekState())->counter == 3);
    INFO("checking correct stateStack size");
    REQUIRE(mMainLoopPtr->stateStack.size() == 1);
    REQUIRE(mMainLoopPtr->changeStatePtr == nullptr);
    mMainLoopPtr->pushState<testState4>(mMainLoopPtr);
    REQUIRE(mMainLoopPtr->changeStatePtr != nullptr);
  }
  void processEvents() {}
  void render(double a) { a++; }
  kge::mainLoop* mMainLoopPtr;
  int counter = 3;
};

class testState4 : public kge::abstractState {
 public:
  testState4(kge::mainLoop* mainLoopRef) : mMainLoopPtr(mainLoopRef) { kgeTestPrint("constructor testState4"); }
  ~testState4() { kgeTestPrint("destructor testState4"); }
  void update(double a) {
    a++;
    INFO("checking correct state is sent");
    REQUIRE(dynamic_cast<testState4*>(mMainLoopPtr->peekState())->counter == 4);
    REQUIRE(dynamic_cast<testState3*>(mMainLoopPtr->peekUnderState())->counter == 3);
    INFO("checking correct stateStack size");
    REQUIRE(mMainLoopPtr->stateStack.size() == 2);
    REQUIRE(mMainLoopPtr->changeStatePtr == nullptr);
    mMainLoopPtr->popPopState();
    REQUIRE(mMainLoopPtr->changeStatePtr != nullptr);
  }
  void processEvents() {}
  void render(double a) { a++; }
  kge::mainLoop* mMainLoopPtr;
  int counter = 4;
};

TEST_CASE("can construct") {
  testData data;
  kge::mainLoop a;

  SECTION("simple tests") {
    a.pushState<testState1>(&a);
    a.run();
    REQUIRE(a.changeStatePtr == nullptr);
  }

  SECTION("more advanced tests") {
    a.pushState<testState2>(&a);
    a.run();
    REQUIRE(a.changeStatePtr == nullptr);
  }
}

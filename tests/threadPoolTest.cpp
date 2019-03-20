/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.73
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

// test typeTrigger

#include "catch2/catch.hpp"

#define private public
#define protected public

#include "kgeThreadPool.h"

int z = 1;
TEST_CASE("thread_pool ") {
  using namespace std::chrono_literals;
  kge::threadPool tp;
  REQUIRE(false == tp.tryJob());  
  for (unsigned i = 0; i < 8; ++i) { // all race conditions but so fast it should be ok
    tp.submitJob([=](){  z++; });
  }
  std::this_thread::sleep_for(10000us); //race condition but should always work.
  REQUIRE(z == 9);
  
  for (unsigned i = 0; i < 8; ++i) { 
    tp.submitJob([=](){ z++; std::this_thread::sleep_for(100000us); });
  }
  REQUIRE(true == tp.tryJob()); 
}


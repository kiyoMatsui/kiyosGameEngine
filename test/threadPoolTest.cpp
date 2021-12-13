/*-------------------------------*\
Copyright 2021 Kiyo Matsui
KiyosGameEngine v2.0
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#include "catch2/catch.hpp"
#include <atomic>

#define private public
#define protected public

#include "kgeThreadPool.h"

std::atomic_int z = 1;
TEST_CASE("thread pool tests") {
  using namespace std::chrono_literals;
  {
    kge::threadPool tp;
    REQUIRE_FALSE(tp.tryJob());
    for (unsigned i = 0; i < 8; ++i) {
      tp.submitJob([=]() { z++; });
    }
    while (z != 9) {
      std::this_thread::yield();
    }
  }  // destroyed here before REQUIRE

  REQUIRE(z == 9);
  kge::threadPool tp;
  for (unsigned i = 0; i < 12; ++i) {
    tp.submitJob([=]() {
      z++;
      std::this_thread::sleep_for(100000us);
    });
  }
  CHECK(tp.tryJob());  // a race conditions but should be ok.
}

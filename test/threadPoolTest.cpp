#include "catch2/catch.hpp"

#define private public
#define protected public

#include "kgeThreadPool.h"

int z = 1;
TEST_CASE("thread pool tests") {
  using namespace std::chrono_literals;
  {
    kge::threadPool tp;
    REQUIRE_FALSE(tp.tryJob());
    for (unsigned i = 0; i < 8; ++i) {
      tp.submitJob([=]() { z++; });
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

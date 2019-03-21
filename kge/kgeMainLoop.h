/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.74
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_MAINLOOP_H
#define KGE_MAINLOOP_H

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace kge {

class abstractState {
public:
  virtual ~abstractState(){};
  virtual void render(const double dt) = 0;
  virtual void update(const double dt) = 0;
  virtual void processEvents() = 0;
};

class mainLoop {
public:
  mainLoop() : stateStack(), changeStatePtr(nullptr) {}

  template <typename pushedState, typename... Elements>
  void pushState(Elements... args);

  void popState() {
    changeStatePtr =
        std::make_unique<std::function<void()>>([=] { mainLoop::popStack(); });
  }

  void popPopState() {
    changeStatePtr = std::make_unique<std::function<void()>>(
        [=] { mainLoop::popPopStack(); });
  }
  template <typename pushedState, typename... Elements>
  void switchState(Elements... args);

  abstractState *peekState() { return stateStack.back().get(); }

  abstractState *peekUnderState() { return stateStack.rbegin()[1].get(); }

  void run() {
    auto start = std::chrono::steady_clock::now();
    if (changeStatePtr) {
      (*changeStatePtr)();
      changeStatePtr = nullptr;
    }
    while (!stateStack.empty()) {
      auto elapsedTime = std::chrono::steady_clock::now() - start;
      start = std::chrono::steady_clock::now();
      double dt_double = std::chrono::duration<double>(elapsedTime).count();
      stateStack.back()->processEvents();
      stateStack.back()->update(dt_double);
      stateStack.back()->render(dt_double);

      if (changeStatePtr) {
        (*changeStatePtr)();
        changeStatePtr = nullptr;
      }
    }
  }

private:
  mainLoop(const mainLoop &) = delete;
  mainLoop &operator=(const mainLoop &) = delete;

  template <typename pushedState, typename... Elements>
  void pushToStack(Elements... args);

  void popStack() { stateStack.pop_back(); }
  void popPopStack() {
    stateStack.pop_back();
    stateStack.pop_back();
  }

  template <typename pushedState, typename... Elements>
  void popAndPushStack(Elements... args);

  std::vector<std::unique_ptr<abstractState>> stateStack;
  std::unique_ptr<std::function<void()>> changeStatePtr;
};

template <typename pushedState, typename... Elements>
void mainLoop::pushState(Elements... args) {
  static_assert(std::is_base_of<abstractState, pushedState>::value,
                "pushedState must be derived from abstractState");
  changeStatePtr = std::make_unique<std::function<void()>>(
      [=] { mainLoop::pushToStack<pushedState, Elements...>(args...); });
}

template <typename pushedState, typename... Elements>
void mainLoop::pushToStack(Elements... args) {
  static_assert(std::is_base_of<abstractState, pushedState>::value,
                "pushedState must be derived from abstractState");
  stateStack.push_back(std::make_unique<pushedState>(args...));
}

template <typename pushedState, typename... Elements>
void mainLoop::switchState(Elements... args) {
  static_assert(std::is_base_of<abstractState, pushedState>::value,
                "pushedState must be derived from abstractState");
  changeStatePtr = std::make_unique<std::function<void()>>(
      [=] { mainLoop::popAndPushStack<pushedState, Elements...>(args...); });
}

template <typename pushedState, typename... Elements>
void mainLoop::popAndPushStack(Elements... args) {
  static_assert(std::is_base_of<abstractState, pushedState>::value,
                "pushedState must be derived from abstractState");
  mainLoop::popStack();
  mainLoop::pushToStack<pushedState, Elements...>(args...);
}

} // namespace kge

#endif

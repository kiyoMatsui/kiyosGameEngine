/*-------------------------------*\
Copyright 2020 Kiyo Matsui
KiyosGameEngine v1.0 
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_ECS_H
#define KGE_ECS_H

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef USED_FOR_KGE_TESTS
#define kgeTestPrint(x) std::cout << x << std::endl
#else
#define kgeTestPrint(x)
#endif

namespace kge {

class entity {
 public:
  explicit entity(const bool aAlive, const unsigned int aID, const int aType) : alive(aAlive), ID(aID), type(aType) {}
  entity(const entity& other) = default;
  entity& operator=(const entity& other) {
    this->alive = other.alive;
    this->type = other.type;
    return *this;
  }
  entity(entity&& other) noexcept = default;
  entity& operator=(entity&& other) noexcept {
    this->alive = other.alive;
    this->type = other.type;
    return *this;
  }
  ~entity() = default;

  bool alive;
  const unsigned int ID;
  int type;
};

class entityHandler {
 public:
  explicit entityHandler(const std::size_t aContainerSize = 10)
      : containerSize(aContainerSize), dummyEntity(false, 999999, -1) {
    container.reserve(containerSize);
    deadStack.reserve(containerSize);
    for (unsigned int number = 0; number < containerSize; number++) {
      container.emplace_back(entity(false, number, -1));
      deadStack.push_back(number);
    }
  }
  entityHandler(const entityHandler& other) = delete;
  entityHandler& operator=(const entityHandler& other) = delete;
  entityHandler(entityHandler&& other) noexcept = delete;
  entityHandler& operator=(entityHandler&& other) noexcept = delete;
  ~entityHandler() = default;

  entity& getItem(const unsigned int a) { return container[a]; }
  entity& getItem(const entity& ent) { return container[ent.ID]; }
  entity& build(const int type) {
    if (!deadStack.empty()) {
      unsigned int id = deadStack.back();
      deadStack.pop_back();
      container[id] = entity(true, id, type);
      return container[id];
    }
    return dummyEntity;
  }

  void destroy(const unsigned int id) {
    container[id].alive = false;
    deadStack.push_back(id);
  }
  void destroy(const entity& ent) {
    container[ent.ID].alive = false;
    deadStack.push_back(ent.ID);
  }

  auto begin() { return container.begin(); }
  auto end() { return container.end(); }

  template <typename Function>
  void for_each(Function f) {
    for (auto& ent : container) {
      if (ent.alive) {
        f(ent);
      }
    }
  }

  template <typename Function>
  void for_each_type(int type, Function f) {
    for (auto& ent : container) {
      if (ent.alive && ent.type == type) {
        f(ent);
      }
    }
  }

  const std::size_t containerSize;

 private:
  std::vector<entity> container;
  std::vector<unsigned int> deadStack;
  entity dummyEntity;
};

template <typename component>
class componentHandler {
 public:
  explicit componentHandler(const entityHandler& aEntityHandler)
      : containerSize(aEntityHandler.containerSize), container(containerSize), dummyComponent() {}

  componentHandler(const componentHandler& other) = delete;
  componentHandler& operator=(const componentHandler& other) = delete;
  componentHandler(componentHandler&& other) noexcept = delete;
  componentHandler& operator=(componentHandler&& other) noexcept = delete;
  ~componentHandler() = default;

  component& getItem(const unsigned int index) {
    if (index > containerSize) {
      return dummyComponent;
    }
    return container[index];
  }
  component& getItem(const entity& ent) {
    if (ent.ID > containerSize) {
      return dummyComponent;
    }
    return container[ent.ID];
  }
  std::vector<component> getData() {
    std::vector<component> temp;
    for (const auto& item : container) {
      temp.push_back(item);
    }
    return std::move(temp);
  }
  auto begin() { return container.begin(); }
  auto end() { return container.end(); }

  const std::size_t containerSize;

 private:
  std::vector<component> container;
  component dummyComponent;
};

template <typename... systems>
class updater {
 public:
  explicit updater(systems&... args) : systemsTuple(std::tie(args...)) { kgeTestPrint("cleanUpSystem constructor"); }

  int update(double dt) {
    eachSystem<sizeof...(systems) - 1>(dt);
    return 1;
  }

 private:
  std::tuple<systems&...> systemsTuple;

  template <unsigned n>
  void eachSystem(double dt) {
    std::get<n>(systemsTuple).update(dt);
    if constexpr (n > 0) {
      eachSystem<n - 1>(dt);
    }
  }
};

}  // namespace kge

#endif

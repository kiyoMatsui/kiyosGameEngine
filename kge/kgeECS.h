/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.7
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

class abstractTrigger {
public:
  virtual ~abstractTrigger() {}
  virtual void trigger() = 0;
  virtual bool reset() = 0;
  virtual bool check() = 0;
};

template <typename T> class typeTrigger : public abstractTrigger {
public:
  virtual void trigger() override { button = true; }

  virtual bool check() override { return button; }

  virtual bool reset() override {
    if (button == false) {
      return false;
    } else {
      button = false;
      return true;
    }
  }

private:
  friend T;
  static inline bool button = false;
};

class baseEntity {
public:
  virtual ~baseEntity() {
    freshBin.push_back(ID);
    typeTrigger<baseEntity> deathTrigger;
    deathTrigger.trigger();
  }

  baseEntity() : ID(getNewID()) {}

  static unsigned int getMaxEntities() { return newID_AndNoOfMaxEntities; }

  static std::vector<unsigned int> getDestroyedIDs() { return freshBin; }

  static void recycleIDs() {
    while (!freshBin.empty()) {
      idBin.push_back(freshBin.back());
      freshBin.pop_back();
    }
    typeTrigger<baseEntity> deathTrigger;
    deathTrigger.reset();
  }

  static unsigned int getNextID() {
    if (idBin.empty()) {
      return newID_AndNoOfMaxEntities;
    } else {
      unsigned int n = idBin.back();
      return n;
    }
  }

  const unsigned int ID;

private:
  unsigned int getNewID() {
    if (idBin.empty()) {
      return newID_AndNoOfMaxEntities++;
    } else {
      unsigned int n = idBin.back();
      idBin.pop_back();
      return n;
    }
  }

  static inline unsigned int newID_AndNoOfMaxEntities = 0;
  static inline std::vector<unsigned int> freshBin;
  static inline std::vector<unsigned int> idBin;
};

template <typename component> class entCompHandler {
  static_assert(!std::is_pointer<component>::value,
                "entCompHandler component is already a unique_ptr");

public:
  entCompHandler() : container(), ID(getNewID()) {
    kgeTestPrint("entCompHandler constructor");
  }

  std::unique_ptr<component> &getItem(unsigned int a) {
    if (baseEntity::getMaxEntities() > container.size()) {
      int newSize = (int)baseEntity::getMaxEntities() - (int)container.size();
      for (auto n = 0; n < newSize; n++) {
        container.push_back(nullptr);
      }
    }
    return container[a];
  }
  void cleanUp() {
    for (auto n : baseEntity::getDestroyedIDs()) {
      container.at(n) = nullptr;
    }
  }

  std::vector<std::unique_ptr<component>> container;
  const unsigned int ID;

private:
  unsigned int getNewID() { return newID_AndNoOfMaxEntities++; }

private:
  static inline unsigned int newID_AndNoOfMaxEntities = 0;
};

class system {
public:
  system() {}
  virtual ~system() {}
  virtual int update(double dt) = 0;
};

template <typename... allComponentHandlers>
class cleanUpSystem : public system {
public:
  cleanUpSystem(allComponentHandlers &... args)
      : componentsTuple(std::tie(args...)) {
    kgeTestPrint("cleanUpSystem constructor");
  }

  int update(double dt) {
    eachComponent<sizeof...(allComponentHandlers) - 1>();
    baseEntity::recycleIDs();
    return 1;
  }

private:
  std::tuple<allComponentHandlers &...> componentsTuple;

  template <unsigned n> void eachComponent() {
    std::get<n>(componentsTuple).cleanUp();
    if constexpr (n > 0) {
      eachComponent<n - 1>();
    }
  }
};

} // namespace kge

#endif
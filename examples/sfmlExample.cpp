/*-------------------------------*\
Copyright 2021 Kiyo Matsui
KiyosGameEngine v1.2 
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "kgeECS.h"
#include "kgeMainLoop.h"
#include "kgePointLine.h"
#include "kgeThreadPool.h"

// Entities
extern constexpr unsigned int startEntity = 1;
extern constexpr unsigned int builtEntity = 2;

// Component (others are points, this one is a little bigger).
class bodyData {
 public:
  bodyData() = default;
  bodyData(double mMass, double aAccelerationX, double aAccelerationY, double aVelocityX, double aVelocityY)
      : mass(mMass), acceleration(aAccelerationX, aAccelerationY), velocity(aVelocityX, aVelocityY) {}

  double mass{0};
  kge::point<double> acceleration{0,0};
  kge::point<double> velocity{0,0};
};

// Systems
class movementSystem {
 public:
  movementSystem(sf::RenderWindow* const aPtr, kge::entityHandler& aEntities,
                 kge::componentHandler<kge::point<double>>& aPosition, kge::componentHandler<bodyData>& aBody,
                 kge::threadPool<4>* const atpPtr)
      : screenSize(aPtr->getSize().x, aPtr->getSize().y),
        entities(aEntities),
        position(aPosition),
        body(aBody),
        mTpPtr(atpPtr) {}

  void updateBlock(const double dt, const int size, const int threadNo, bool endFlag = false) {
    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_int_distribution<int> r0(0, 9);
    int block = size * threadNo;
    auto iterEnd = entities.begin() + block + size;
    if (endFlag) {
      iterEnd = entities.end();
    }
    for (auto iter = entities.begin() + block; iter != iterEnd; iter++) {
      if (iter->alive) {
        bool killFlag = false;
        body.getItem(iter->ID).velocity *= 0.5;
        body.getItem(iter->ID).velocity += (body.getItem(iter->ID).acceleration * dt);
        for (auto iter2 = entities.begin(); iter2 != entities.end(); iter2++) {
          if (iter2->alive) {
            if (entities.getItem(iter2->ID).alive && iter2->ID != iter->ID) {
              kge::line temp(copyCont[iter->ID], copyCont[iter2->ID]);
              if (temp.length() < 20.0) {
                kge::point<double> directionVec = copyCont[iter->ID] - copyCont[iter2->ID];
                body.getItem(iter->ID).velocity +=
                  ((directionVec /= std::hypot(directionVec.x, directionVec.y)) * (1000 / temp.length()));
              }
            }
          }
        }
        if (std::hypot(body.getItem(iter->ID).velocity.x, body.getItem(iter->ID).velocity.y) > 100.0) {
          body.getItem(iter->ID).velocity /=
            std::hypot(body.getItem(iter->ID).velocity.x, body.getItem(iter->ID).velocity.y);
          body.getItem(iter->ID).velocity *= 100.0;
        }

        position.getItem(iter->ID) += body.getItem(iter->ID).velocity * dt;
        if ((position.getItem(iter->ID).x < 10 && body.getItem(iter->ID).velocity.x < 0) ||
            (position.getItem(iter->ID).x > screenSize.x - 10 && body.getItem(iter->ID).velocity.x > 0)) {
          if (r0(rng) > 1) {
            body.getItem(iter->ID).velocity.x *= -1;
            body.getItem(iter->ID).acceleration.x *= -2;
          } else {
            killFlag = true;
          }
        }
        if ((position.getItem(iter->ID).y < 10 && body.getItem(iter->ID).velocity.y < 0) ||
            (position.getItem(iter->ID).y > screenSize.y - 10 && body.getItem(iter->ID).velocity.y > 0)) {
          if (r0(rng) > 1) {
            body.getItem(iter->ID).velocity.y *= -1;
            body.getItem(iter->ID).acceleration.y *= -2;
          } else {
            killFlag = true;
          }
        }
        if (killFlag) {
          std::lock_guard<std::mutex> lock(mKillSetMutex);
          mKillSet.insert(iter->ID);
        }
      }
    }
    sync--;
  }

  int update(const double dt) {
    copyCont = position.getData();

    unsigned int block = static_cast<unsigned int>(entities.containerSize) / 4;
    for (int x = 0; x < 3; x++) {
      sync++;
      mTpPtr->submitJob([=]() { updateBlock(dt, block, x); });
    }
    sync++;
    updateBlock(dt, block, 3, true);

    while (sync.load() != 0) {
      std::this_thread::yield();
    }
    for (auto a : mKillSet) {
      entities.destroy(a);
    }
    mKillSet.clear();
    var = 0;
    return 1;
  }

 private:
  kge::point<double> screenSize;
  kge::entityHandler& entities;
  kge::componentHandler<kge::point<double>>& position;
  kge::componentHandler<bodyData>& body;
  kge::threadPool<4>* const mTpPtr;
  std::unordered_set<unsigned int> mKillSet;
  mutable std::mutex mKillSetMutex;
  std::atomic_int sync{0};
  std::atomic_int var{0};
  std::vector<kge::point<double>> copyCont;
};

class renderSystem {
 public:
  renderSystem(sf::RenderWindow* const aPtr, kge::entityHandler& aEntities,
               kge::componentHandler<kge::point<double>>& aPosition, kge::componentHandler<sf::ConvexShape>& aMeteor)
      : wPtr(aPtr), entities(aEntities), position(aPosition), meteor(aMeteor) {}

  int update(const double dt) {
    timer += dt;
    if (timer > 3.0) {
      entities.for_each_type(2, [&](auto& ent) {
        timer = 0.0;
        meteor.getItem(ent.ID).setFillColor(sf::Color::White);
        std::random_device rnd;
        std::mt19937 rng(rnd());
        std::uniform_int_distribution<unsigned char> r0(0, 255);
        meteor.getItem(ent.ID).setFillColor(sf::Color(r0(rng), r0(rng), r0(rng)));
      });
    }

    entities.for_each([&](auto& ent) {
      if (ent.alive) {
        meteor.getItem(ent.ID).setPosition((float)position.getItem(ent.ID).x, (float)position.getItem(ent.ID).y);
        wPtr->draw(meteor.getItem(ent.ID));
      }
    });
    return 1;
  }

 private:
  sf::RenderWindow* const wPtr;
  kge::entityHandler& entities;
  kge::componentHandler<kge::point<double>>& position;
  kge::componentHandler<sf::ConvexShape>& meteor;
  double timer{0};
};

// spawn system

class spawnSystem {
 public:
  spawnSystem(sf::RenderWindow* const aPtr, kge::entityHandler& aEntities,
              kge::componentHandler<kge::point<double>>& aPosition, kge::componentHandler<bodyData>& aBody,
              kge::componentHandler<sf::ConvexShape>& aMeteor)
      : screenSize(aPtr->getSize().x, aPtr->getSize().y),
        entities(aEntities),
        position(aPosition),
        body(aBody),
        meteor(aMeteor) {}

  int update(const double /*unused*/) {
    for (auto& j : spawnStack) {
      unsigned int builtID = entities.build(builtEntity).ID;
      std::random_device rnd;
      std::mt19937 rng(rnd());
      std::uniform_real_distribution<double> r0(10, screenSize.x - 10);
      std::uniform_real_distribution<double> r1(10, screenSize.y - 10);
      std::uniform_real_distribution<float> r2(0, 360);
      std::uniform_real_distribution<float> r3(-2000, 2000);
      position.getItem(builtID) = kge::point<double>(j.x, j.y);
      body.getItem(builtID) = bodyData(1.0, r3(rng), r3(rng), 0.0, 0.0);
      meteor.getItem(builtID) = sf::ConvexShape();
      meteor.getItem(builtID).setPointCount(5);
      meteor.getItem(builtID).setPoint(0, sf::Vector2f(10, 0));
      meteor.getItem(builtID).setPoint(1, sf::Vector2f(18, 9));
      meteor.getItem(builtID).setPoint(2, sf::Vector2f(15, 19));
      meteor.getItem(builtID).setPoint(3, sf::Vector2f(6, 18));
      meteor.getItem(builtID).setPoint(4, sf::Vector2f(2, 8));
      meteor.getItem(builtID).setOrigin(10, 10);
      meteor.getItem(builtID).rotate(r2(rng));
      meteor.getItem(builtID).setFillColor(sf::Color::Red);
      meteor.getItem(builtID).setPosition((float)position.getItem(builtID).x, (float)position.getItem(builtID).y);
    }
    spawnStack.clear();
    return 1;
  }
  std::vector<kge::point<double>> spawnStack;

 private:
  int spawn(double x, double y);

 private:
  kge::point<double> screenSize;
  kge::entityHandler& entities;
  kge::componentHandler<kge::point<double>>& position;
  kge::componentHandler<bodyData>& body;
  kge::componentHandler<sf::ConvexShape>& meteor;
};

// states

class pauseState final : public kge::abstractState {
 public:
  pauseState(sf::RenderWindow* const aPtr, kge::mainLoop* const mainLoopPtr, kge::entityHandler* aEntities,
             kge::componentHandler<sf::ConvexShape>* aMeteor)
      : wPtr(aPtr), mMainLoopPtr(mainLoopPtr), entitiesPtr(aEntities), meteorPtr(aMeteor) {
    if (!font.loadFromFile("../thirdParty/LiberationSans-Regular.ttf")) {
      font.loadFromFile("thirdParty/LiberationSans-Regular.ttf");
    }
    text.setFont(font);
    text.setPosition(sf::Vector2f((float)(aPtr->getSize().x) / 8, (float)(aPtr->getSize().y) / 8));
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setString("spacebar to resume    Esc to quit");
  }

  pauseState(const pauseState& other) = delete;
  pauseState& operator=(const pauseState& other) = delete;
  pauseState(pauseState&& other) noexcept = delete;
  pauseState& operator=(pauseState&& other) noexcept = delete;
  ~pauseState() override = default;

  void update(const double /*unused*/) override {}

  void processEvents() override {
    sf::Event event;
    wPtr->pollEvent(event);
    switch (event.type) {
      case sf::Event::Closed:
        mMainLoopPtr->popPopState();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Space) {
          mMainLoopPtr->popState();
        } else if (event.key.code == sf::Keyboard::Escape) {
          mMainLoopPtr->popPopState();
        }
        break;

      default:
        break;
    }
  }

  void render(const double /*unused*/) override {
    wPtr->clear(sf::Color::Black);
    sf::RectangleShape greyBack;
    greyBack.setFillColor(sf::Color(0, 0, 0, 200));
    greyBack.setSize(wPtr->getView().getSize());
    entitiesPtr->for_each([&](auto& ent) {
      if (ent.alive) {
        wPtr->draw(meteorPtr->getItem(ent.ID));
      }
    });
    wPtr->draw(greyBack);
    wPtr->draw(text);
    wPtr->display();
  }

  sf::RenderWindow* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::entityHandler* entitiesPtr;
  kge::componentHandler<sf::ConvexShape>* meteorPtr;
  sf::Font font;
  sf::Text text;
};

class gameState final : public kge::abstractState {
 public:
  gameState(sf::RenderWindow* const aPtr, kge::mainLoop* const mainLoopPtr, kge::threadPool<4>* const atpPtr)
      : wPtr(aPtr),
        mMainLoopPtr(mainLoopPtr),
        mTpPtr(atpPtr),
        entities(110),
        position(entities),
        body(entities),
        meteor(entities),
        mMoveSystem(aPtr, entities, position, body, atpPtr),
        mRenderSystem(aPtr, entities, position, meteor),
        mSpawnSystem(aPtr, entities, position, body, meteor),
        mUpdater(mMoveSystem, mSpawnSystem) {
    for (unsigned int i = 0; i < 100; i++) {
      unsigned int builtID = entities.build(startEntity).ID;
      std::random_device rnd;
      std::mt19937 rng(rnd());
      std::uniform_real_distribution<double> r0(10, aPtr->getSize().x - 10);
      std::uniform_real_distribution<double> r1(10, aPtr->getSize().y - 10);
      std::uniform_real_distribution<float> r2(0, 360);
      std::uniform_real_distribution<float> r3(-2000, 2000);
      position.getItem(builtID) = kge::point<double>(r0(rng), r1(rng));
      body.getItem(builtID) = bodyData(1.0, r3(rng), r3(rng), 0.0, 0.0);
      meteor.getItem(builtID).setPointCount(5);
      meteor.getItem(builtID).setPoint(0, sf::Vector2f(10, 0));
      meteor.getItem(builtID).setPoint(1, sf::Vector2f(18, 9));
      meteor.getItem(builtID).setPoint(2, sf::Vector2f(15, 19));
      meteor.getItem(builtID).setPoint(3, sf::Vector2f(6, 18));
      meteor.getItem(builtID).setPoint(4, sf::Vector2f(2, 8));
      meteor.getItem(builtID).setOrigin(10, 10);
      meteor.getItem(builtID).rotate(r2(rng));
      meteor.getItem(builtID).setFillColor(sf::Color::White);
      meteor.getItem(builtID).setPosition((float)position.getItem(builtID).x, (float)position.getItem(builtID).y);
    }
  }

  gameState(const gameState& other) = delete;
  gameState& operator=(const gameState& other) = delete;
  gameState(gameState&& other) noexcept = delete;
  gameState& operator=(gameState&& other) noexcept = delete;
  ~gameState() override = default;

  void update(const double dt) override { mUpdater.update(dt); }

  void processEvents() override {
    sf::Event event;
    wPtr->pollEvent(event);
    switch (event.type) {
      case sf::Event::Closed:
        mMainLoopPtr->popState();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Space) {
          mMainLoopPtr->pushState<pauseState, sf::RenderWindow* const, kge::mainLoop* const, kge::entityHandler*,
                                  kge::componentHandler<sf::ConvexShape>*>(wPtr, mMainLoopPtr, &entities, &meteor);
        } else if (event.key.code == sf::Keyboard::Escape) {
          mMainLoopPtr->popState();
        }
        break;
      case sf::Event::MouseButtonPressed:
        mSpawnSystem.spawnStack.emplace_back(event.mouseButton.x, event.mouseButton.y);
        break;
      default:
        break;
    }
  }

  void render(const double dt) override {
    wPtr->clear(sf::Color::Black);
    mRenderSystem.update(dt);
    wPtr->display();
  }

  sf::RenderWindow* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::threadPool<4>* const mTpPtr;
  kge::entityHandler entities;
  kge::componentHandler<kge::point<double>> position;
  kge::componentHandler<bodyData> body;
  kge::componentHandler<sf::ConvexShape> meteor;
  movementSystem mMoveSystem;
  renderSystem mRenderSystem;
  spawnSystem mSpawnSystem;
  kge::updater<movementSystem, spawnSystem> mUpdater;
};

class menuState final : public kge::abstractState {
 public:
  menuState(sf::RenderWindow* const aPtr, kge::mainLoop* const mainLoopPtr, kge::threadPool<4>* const atpPtr)
      : wPtr(aPtr), mMainLoopPtr(mainLoopPtr), mTpPtr(atpPtr) {
    if (!font.loadFromFile("../thirdParty/LiberationSans-Regular.ttf")) {
      font.loadFromFile("thirdParty/LiberationSans-Regular.ttf");
    }
    text.setFont(font);
    text.setPosition(sf::Vector2f((float)(aPtr->getSize().x) / 8, (float)(aPtr->getSize().y) / 8));
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setString("thanks for trying kge,    spacebar to play/pause demo    Esc to quit");
  }

  menuState(const menuState& other) = delete;
  menuState& operator=(const menuState& other) = delete;
  menuState(menuState&& other) noexcept = delete;
  menuState& operator=(menuState&& other) noexcept = delete;
  ~menuState() override = default;

  void update(double /*unused*/) override {}

  void processEvents() override {
    sf::Event event;
    wPtr->pollEvent(event);
    switch (event.type) {
      case sf::Event::Closed:
        mMainLoopPtr->popState();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Space) {
          mMainLoopPtr
            ->switchState<gameState, sf::RenderWindow* const, kge::mainLoop* const, kge::threadPool<4>* const>(
              wPtr, mMainLoopPtr, mTpPtr);
        } else if (event.key.code == sf::Keyboard::Escape) {
          mMainLoopPtr->popState();
        }
        break;

      default:
        break;
    }
  }

  void render(double /*unused*/) override {
    wPtr->clear(sf::Color::Black);
    wPtr->draw(text);
    wPtr->display();
  }

  sf::RenderWindow* const wPtr;
  kge::mainLoop* mMainLoopPtr;
  kge::threadPool<4>* const mTpPtr;
  sf::Font font;
  sf::Text text;
};

int main() {
  sf::RenderWindow mWindow(sf::VideoMode(800, 600), "kge example");
  mWindow.setFramerateLimit(60);
  mWindow.setVerticalSyncEnabled(true);
  kge::threadPool<4> mThreadPool;
  sf::RenderWindow* const mPtr = &mWindow;
  kge::threadPool<4>* const tpPtr = &mThreadPool;
  kge::mainLoop myGame;
  kge::mainLoop* const gPtr = &myGame;
  myGame.pushState<menuState, sf::RenderWindow* const, kge::mainLoop* const, kge::threadPool<4>* const>(mPtr, gPtr,
                                                                                                        tpPtr);
  myGame.run();

  return 0;
}

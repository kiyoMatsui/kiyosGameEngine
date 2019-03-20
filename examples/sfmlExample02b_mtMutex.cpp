/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.73
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#include <iostream>
#include <cmath>
#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <random>
#include <shared_mutex>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "kgeMainLoop.h"
#include "kgePointLine.h"
#include "kgeECS.h"
#include "kgeThreadPool.h"

class createdEntity;

class bodyData {
public:
  explicit bodyData( double mMass,
                     double aAccelerationX,
                     double aAccelerationY,
                     double aVelocityX,
                     double aVelocityY)
      : mass(mMass)
      , acceleration(aAccelerationX, aAccelerationY)
      , velocity(aVelocityX, aVelocityY) {}
  
  double mass;
  kge::point<double> acceleration;
  kge::point<double> velocity;
  std::shared_mutex sm;
};

class movementSystem : public kge::system {
public:
  movementSystem(sf::RenderWindow* const aPtr,
                 kge::entCompHandler<kge::baseEntity>& aEntities,
                 kge::entCompHandler<kge::point<double>>& aPosition,
                 kge::entCompHandler<bodyData>& aBody,
                 kge::threadPool<4>* const atpPtr ) 
    : screenSize(aPtr->getSize().x, aPtr->getSize().y)
    , entities(aEntities)
    , position(aPosition)
    , body(aBody)
    , mTpPtr(atpPtr) { }

 
  void updateBlock(double dt,
                   unsigned int size,
                   unsigned int threadNo,
                   bool endFlag = false) {
    #define posPtr position.getItem((*iter)->ID)
    #define bodyPtr body.getItem((*iter)->ID)
    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_int_distribution<int> r0(0,9);
    unsigned int block = size * threadNo;
    auto iterEnd=entities.container.begin() + block + size;
    if(endFlag == true) iterEnd=entities.container.end();
    for (auto iter=entities.container.begin() + block; iter!=iterEnd; iter++) {
      if(iter->get()) {
        if (position.getItem((*iter)->ID)
            && body.getItem((*iter)->ID)) {
          bool killFlag = false;
          bodyPtr->velocity *= 0.5;
          bodyPtr->velocity += (bodyPtr->acceleration * dt);
          for (auto iter2=entities.container.begin(); iter2!=entities.container.end(); iter2++) {
            if(iter2->get()) {
              if (position.getItem((*iter2)->ID) && (*iter2)->ID != (*iter)->ID) {
                if ((*iter)->ID > (*iter2)->ID) {
                  body.getItem((*iter2)->ID)->sm.lock_shared();
                  body.getItem((*iter)->ID)->sm.lock();
                } else {
                  body.getItem((*iter)->ID)->sm.lock();
                  body.getItem((*iter2)->ID)->sm.lock_shared();
                }      
                kge::line temp(*posPtr, *position.getItem((*iter2)->ID));
                if (temp.length() < 20.0) {
                  kge::line temp(*posPtr, *position.getItem((*iter2)->ID));
                  kge::point<double> directionVec = *posPtr - *position.getItem((*iter2)->ID);
                  bodyPtr->velocity += ((directionVec /= std::hypot(directionVec.x, directionVec.y)) * (1000/temp.length()));
                }
                body.getItem((*iter)->ID)->sm.unlock();
                body.getItem((*iter2)->ID)->sm.unlock();
              }
            } 
          }

          body.getItem((*iter)->ID)->sm.lock();
          if (std::hypot(bodyPtr->velocity.x, bodyPtr->velocity.y) > 100.0) {
            bodyPtr->velocity /= std::hypot(bodyPtr->velocity.x, bodyPtr->velocity.y);
            bodyPtr->velocity *= 100.0;
          }

          *posPtr += bodyPtr->velocity * dt;
          if ((posPtr->x  < 10 && bodyPtr->velocity.x < 0)
              || (posPtr->x > screenSize.x-10 && bodyPtr->velocity.x > 0)) {
            if (r0(rng) > 1) {
              (*bodyPtr).velocity.x *= -1;
              (*bodyPtr).acceleration.x *= -2;
            } else
              killFlag = true;
          }
          if ((posPtr->y  < 10 && bodyPtr->velocity.y < 0)
              || (posPtr->y > screenSize.y-10 && bodyPtr->velocity.y > 0)) {
            if (r0(rng) > 1) {
              (*bodyPtr).velocity.y *= -1;
              (*bodyPtr).acceleration.y *= -2;
            } else
              killFlag = true;
          }
          body.getItem((*iter)->ID)->sm.unlock();
          if(killFlag) {
            std::lock_guard<std::mutex> lock(mKillSetMutex);
            mKillSet.insert((*iter)->ID);
          }
        } 
      }
    }
  sync--;
  }
  
  int update(double dt) override {
    unsigned int block = (unsigned int)entities.container.size() / 4;
    for ( int x = 0; x < 3; x++ ) {
      sync++;
      mTpPtr->submitJob([=](){ updateBlock(dt,block,x); });
    }
    sync++;
    updateBlock(dt,block,3,true);

    while (sync.load() != 0) {
      std::this_thread::yield();
    }
    for (auto a : mKillSet) {
      entities.getItem(a).reset(nullptr);
    }
    mKillSet.clear();
    return 1;
  }

 private:
  kge::point<double> screenSize;
  kge::entCompHandler<kge::baseEntity>& entities;
  kge::entCompHandler<kge::point<double>>& position;
  kge::entCompHandler<bodyData>& body;
  kge::threadPool<4>* const mTpPtr;
  std::unordered_set<unsigned int> mKillSet;
  mutable std::mutex mKillSetMutex;
  std::atomic_int sync = 0; 
};

class renderSystem : public kge::system {
public:
  renderSystem(sf::RenderWindow* const aPtr,
               kge::entCompHandler<kge::baseEntity>& aEntities,
               kge::entCompHandler<kge::point<double>>& aPosition,
               kge::entCompHandler<sf::ConvexShape>& aMeteor)
      : wPtr(aPtr)
      , entities(aEntities)
      , position(aPosition)
      , meteor(aMeteor) {}
  
  int update(double dt) {
    timer += dt;
    if (timer > 3.0 ) {
      for( auto n : createdMeteorIDs) {
          timer = 0.0;
          meteor.getItem(n)->setFillColor(sf::Color::White);
                std::random_device rnd;
                std::mt19937 rng(rnd());
                std::uniform_int_distribution<unsigned char> r0(0, 255);
                meteor.getItem(n)->setFillColor(sf::Color(r0(rng), r0(rng), r0(rng)));
      }
      createdMeteorIDs.clear();
    }
  #define posPtr2 position.getItem(i->ID)
  #define meteorPtr meteor.getItem(i->ID)
    for ( auto& i : entities.container) {
      if(i.get()) {
        if (position.getItem(i->ID)
          && meteor.getItem(i->ID)) {
          meteorPtr->setPosition((float)posPtr2->x, (float)posPtr2->y);
          wPtr->draw(*meteorPtr);
        }
      }
    }
    return 1;
}

  friend createdEntity;
 private:
  sf::RenderWindow* const wPtr;
  kge::entCompHandler<kge::baseEntity>& entities;
  kge::entCompHandler<kge::point<double>>& position;
  kge::entCompHandler<sf::ConvexShape>& meteor;
  double timer = 0;
  static inline std::unordered_set<unsigned int> createdMeteorIDs;
};

//entities

class mainEntity : public kge::baseEntity {
public:
  mainEntity(int a) : exampleData(a) {}
  
   int exampleData;
};

class createdEntity : public kge::baseEntity {
public:
  createdEntity(int a) : exampleData(a) {
    renderSystem::createdMeteorIDs.insert(ID);
      }
  ~createdEntity()  {
    renderSystem::createdMeteorIDs.erase(ID);
 
      }

  int exampleData;
};

//spawn system

class spawnSystem : public kge::system {
public:
  spawnSystem(sf::RenderWindow* const aPtr,
              kge::entCompHandler<kge::baseEntity>& aEntities,
              kge::entCompHandler<kge::point<double>>& aPosition,
              kge::entCompHandler<bodyData>& aBody,
              kge::entCompHandler<sf::ConvexShape>& aMeteor) 
    : screenSize(aPtr->getSize().x, aPtr->getSize().y)
    , entities(aEntities)
    , position(aPosition)
    , body(aBody)
    , meteor(aMeteor) { }
  
  int update(double dt) {
    for ( auto& j : spawnStack) {
      unsigned int i = kge::baseEntity::getNextID();
      entities.getItem(i) = std::make_unique<createdEntity>(42);
      std::random_device rnd;
      std::mt19937 rng(rnd());
      std::uniform_real_distribution<double> r0(10, screenSize.x-10);
      std::uniform_real_distribution<double> r1(10, screenSize.y-10);
      std::uniform_real_distribution<float> r2(0,360); 
      std::uniform_real_distribution<float> r3(-2000,2000); 
      position.getItem(i) = std::make_unique<kge::point<double>>(j.x, j.y);
      body.getItem(i) = std::make_unique<bodyData>(1.0, r3(rng), r3(rng), 0.0, 0.0);
      meteor.getItem(i) = std::make_unique<sf::ConvexShape>();
      meteor.getItem(i)->setPointCount(5);
      meteor.getItem(i)->setPoint(0, sf::Vector2f(10, 0));
      meteor.getItem(i)->setPoint(1, sf::Vector2f(18, 9));
      meteor.getItem(i)->setPoint(2, sf::Vector2f(15, 19));
      meteor.getItem(i)->setPoint(3, sf::Vector2f(6, 18));
      meteor.getItem(i)->setPoint(4, sf::Vector2f(2, 8));
      meteor.getItem(i)->setOrigin(10, 10);
      meteor.getItem(i)->rotate(r2(rng));
      meteor.getItem(i)->setFillColor(sf::Color::Red);
      meteor.getItem(i)->setPosition(
          (float)position.getItem(i)->x,
          (float)position.getItem(i)->y);
    }
    spawnStack.clear();
    return 1;
  }
  std::vector<kge::point<double>> spawnStack; 
 private:
  int spawn(double x, double y);
  
 private:
  kge::point<double> screenSize;
  kge::entCompHandler<kge::baseEntity>& entities;
  kge::entCompHandler<kge::point<double>>& position;
  kge::entCompHandler<bodyData>& body;
  kge::entCompHandler<sf::ConvexShape>& meteor;
};


//states

class pauseState : public kge::abstractState {
 public:
  pauseState(sf::RenderWindow* const aPtr, kge::mainLoop* const mainLoopPtr, kge::entCompHandler<sf::ConvexShape>* aMetPtr)
  : wPtr(aPtr)
  , mMainLoopPtr(mainLoopPtr)
  , metPtr(aMetPtr) {
    if(!font.loadFromFile("../thirdParty/LiberationSans-Regular.ttf")) {
      font.loadFromFile("thirdParty/LiberationSans-Regular.ttf");
    }
    text.setFont(font);
    text.setPosition(sf::Vector2f((float)(aPtr->getSize().x)/8, (float)(aPtr->getSize().y)/8));
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setString("spacebar to resume    Esc to quit"); 
  }
    
  ~pauseState() {}
  
  void update(double dt) {

  }
  
  void processEvents() {
    sf::Event event; 
    wPtr->pollEvent(event);
      switch (event.type) {
        case sf::Event::Closed:
          mMainLoopPtr->popPopState();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Space) {
          mMainLoopPtr->popState();
          }
          if (event.key.code == sf::Keyboard::Escape) {
          mMainLoopPtr->popPopState();
          }
          break;
default:
          break;
      }
  }
 
  void render(double dt) {
    wPtr->clear(sf::Color::Black);
    sf::RectangleShape greyBack;
    greyBack.setFillColor(sf::Color(0, 0, 0, 200));
    greyBack.setSize(wPtr->getView().getSize());
    for ( auto& i : metPtr->container) {
      if(i.get()) {
          wPtr->draw(*i);
      }
    }
    wPtr->draw(greyBack);
    wPtr->draw(text);
    wPtr->display();
  }
  
  sf::RenderWindow* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::entCompHandler<sf::ConvexShape>* metPtr;
  sf::Font font;
  sf::Text text;
            
};

class gameState : public kge::abstractState {
 public:
  gameState(sf::RenderWindow* const aPtr,
            kge::mainLoop* const mainLoopPtr,
            kge::threadPool<4>* const atpPtr)
  : wPtr(aPtr)
  , mMainLoopPtr(mainLoopPtr)
  , mTpPtr(atpPtr)  
  , position()
  , body()
  , meteor()
  , mMoveSystem(aPtr, entities, position, body, atpPtr)
  , mRenderSystem(aPtr, entities, position, meteor)
  , mSpawnSystem(aPtr, entities, position, body, meteor)
  , mCleanupSystem(entities, position, body, meteor) {
    for (unsigned int i = 0; i < 100; i++) {
      entities.getItem(i) = std::make_unique<mainEntity>(42);
      std::random_device rnd;
      std::mt19937 rng(rnd());
      std::uniform_real_distribution<double> r0(10, aPtr->getSize().x-10);
      std::uniform_real_distribution<double> r1(10, aPtr->getSize().y-10);
      std::uniform_real_distribution<float> r2(0,360); 
      std::uniform_real_distribution<float> r3(-2000,2000); 
      position.getItem(i) = std::make_unique<kge::point<double>>(r0(rng), r1(rng));
      body.getItem(i) = std::make_unique<bodyData>(1.0, r3(rng), r3(rng), 0.0, 0.0);
      meteor.getItem(i) = std::make_unique<sf::ConvexShape>();
      meteor.getItem(i)->setPointCount(5);
      meteor.getItem(i)->setPoint(0, sf::Vector2f(10, 0));
      meteor.getItem(i)->setPoint(1, sf::Vector2f(18, 9));
      meteor.getItem(i)->setPoint(2, sf::Vector2f(15, 19));
      meteor.getItem(i)->setPoint(3, sf::Vector2f(6, 18));
      meteor.getItem(i)->setPoint(4, sf::Vector2f(2, 8));
      meteor.getItem(i)->setOrigin(10, 10);
      meteor.getItem(i)->rotate(r2(rng));
      meteor.getItem(i)->setFillColor(sf::Color::White);
      meteor.getItem(i)->setPosition(
          (float)position.getItem(i)->x,
          (float)position.getItem(i)->y);
    }  
  }
    
  ~gameState() {}
  
  void update(double dt) {
    auto start = std::chrono::high_resolution_clock::now();
    mMoveSystem.update(dt);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " microseconds\n";
    
    mSpawnSystem.update(dt);
    mCleanupSystem.update(dt);
  }
  
  void processEvents() {
    sf::Event event; 
    wPtr->pollEvent(event);
      switch (event.type) {
        case sf::Event::Closed:
          mMainLoopPtr->popState();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Space) {
            mMainLoopPtr->pushState<pauseState, sf::RenderWindow* const,
                                    kge::mainLoop* const,
                                    kge::entCompHandler<sf::ConvexShape>*>(wPtr, mMainLoopPtr, &meteor);
          }
          if (event.key.code == sf::Keyboard::Escape) {
            mMainLoopPtr->popState();
          }
          break;
        case sf::Event::MouseButtonPressed:
          mSpawnSystem.spawnStack.emplace_back(
              event.mouseButton.x,
              event.mouseButton.y);
          break;          
        default:
          break;
      }
  }
 
  void render(double dt) {
    wPtr->clear(sf::Color::Black);
    mRenderSystem.update(dt);
    wPtr->display();
  }

  sf::RenderWindow* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::threadPool<4>* const mTpPtr;
  kge::entCompHandler<kge::baseEntity> entities;
  kge::entCompHandler<kge::point<double>> position;
  kge::entCompHandler<bodyData> body;
  kge::entCompHandler<sf::ConvexShape> meteor;
  movementSystem mMoveSystem;
  renderSystem mRenderSystem;
  spawnSystem mSpawnSystem;
  kge::cleanUpSystem<kge::entCompHandler<kge::baseEntity>,
                kge::entCompHandler<kge::point<double>>,
                kge::entCompHandler<bodyData>,
                kge::entCompHandler<sf::ConvexShape> >
  mCleanupSystem;               
};

class menuState : public kge::abstractState {
 public:
  menuState(sf::RenderWindow* const aPtr,
            kge::mainLoop* const mainLoopPtr,
            kge::threadPool<4>* const atpPtr)
  : wPtr(aPtr)
  , mMainLoopPtr(mainLoopPtr)
  , mTpPtr(atpPtr) {
    if(!font.loadFromFile("../thirdParty/LiberationSans-Regular.ttf")) {
      font.loadFromFile("thirdParty/LiberationSans-Regular.ttf");
    }
    text.setFont(font);
    text.setPosition(sf::Vector2f((float)(aPtr->getSize().x)/8, (float)(aPtr->getSize().y)/8));
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setString("thanks for trying kge,    spacebar to play/pause demo    Esc to quit");
  }
    
  ~menuState() {}
  
  void update(double dt) {

  }
  
  void processEvents() {
    sf::Event event; 
    wPtr->pollEvent(event);
      switch (event.type) {
        case sf::Event::Closed:
          mMainLoopPtr->popState();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Space) {
          mMainLoopPtr->switchState<gameState,
                                    sf::RenderWindow* const,
                                    kge::mainLoop* const,
                                    kge::threadPool<4>* const>(wPtr, mMainLoopPtr, mTpPtr);
          }
          if (event.key.code == sf::Keyboard::Escape) {
          mMainLoopPtr->popState();
          }
          break;
        
        default:
          break;
      }
  }
 
  void render(double dt) {
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
  myGame.pushState<menuState,
                   sf::RenderWindow* const,
                   kge::mainLoop* const,
                   kge::threadPool<4>* const>(mPtr, gPtr, tpPtr);
  myGame.run();

  return 0;
}

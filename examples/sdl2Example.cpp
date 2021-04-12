#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>
#include <stdexcept>

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "kgeECS.h"
#include "kgeMainLoop.h"
#include "kgePointLine.h"
#include "kgeThreadPool.h"

// Entities
#define startEntity 1
#define builtEntity 2

// Component (others are points, this one is a little bigger).
class bodyData {
 public:
  bodyData() {}
  bodyData(double mMass, double aAccelerationX, double aAccelerationY, double aVelocityX, double aVelocityY)
      : mass(mMass), acceleration(aAccelerationX, aAccelerationY), velocity(aVelocityX, aVelocityY) {}

  double mass{0};
  kge::point<double> acceleration{0,0};
  kge::point<double> velocity{0,0};
};

class meteorShape {
 public:
  meteorShape() {}
  meteorShape(SDL_color aColour, SDL_Rect aRect)
      : colour(0, 0, 0, 255), rect(0,0,1,1) {}

  create() {
      rect.x = 0; 
      rect.y = 0; 
      rect.w = 14; 
      rect.h = 14; 
      colour = { 255, 255, 255, 255};

  }

  setFillColor(SDL_color aColour) {
    colour = aColour;
  }
  
  SDL_color colour;
  SDL_Rect rect;
};

// Systems
class movementSystem {
 public:
  movementSystem(SDLpointers mContext* const aPtr, kge::entityHandler& aEntities,
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
  renderSystem(SDLpointers mContext* const aPtr, kge::entityHandler& aEntities,
               kge::componentHandler<kge::point<double>>& aPosition, kge::componentHandler<meteorShape>& aMeteor)
      : wPtr(aPtr), entities(aEntities), position(aPosition), meteor(aMeteor) {}

  int update(const double dt) {
    timer += dt;
    if (timer > 3.0) {
      entities.for_each_type(2, [&](auto& ent) {
        timer = 0.0;
        std::random_device rnd;
        std::mt19937 rng(rnd());
        std::uniform_int_distribution<unsigned char> r0(0, 255);
        SDL_color mColour = { r0(rng), r0(rng), r0(rng) };
        meteor.getItem(ent.ID).setFillColor(mColour);
      });
    }

    entities.for_each([&](auto& ent) {
      if (ent.alive) { //////////////////////////////////////////////////////
        meteor.getItem(ent.ID).setPosition((float)position.getItem(ent.ID).x, (float)position.getItem(ent.ID).y);
        wPtr->draw(meteor.getItem(ent.ID));
      }
    });
    return 1;
  }

 private:
  SDLpointers mContext* const wPtr;
  kge::entityHandler& entities;
  kge::componentHandler<kge::point<double>>& position;
  kge::componentHandler<meteorShape>& meteor;
  double timer{0};
};

// spawn system

class spawnSystem {
 public:
  spawnSystem(SDLpointers mContext* const aPtr, kge::entityHandler& aEntities,
              kge::componentHandler<kge::point<double>>& aPosition, kge::componentHandler<bodyData>& aBody,
              kge::componentHandler<meteorShape>& aMeteor)
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
      std::uniform_real_distribution<float> r3(-2000, 2000);
      position.getItem(builtID) = kge::point<double>(j.x, j.y);
      body.getItem(builtID) = bodyData(1.0, r3(rng), r3(rng), 0.0, 0.0);
      meteor.getItem(builtID).create();
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
  kge::componentHandler<meteorShape>& meteor;
};

// states

class pauseState final : public kge::abstractState {
 public:
  pauseState(SDLpointers mContext* const aPtr, kge::mainLoop* const mainLoopPtr, kge::entityHandler* aEntities,
             kge::componentHandler<meteorShape>* aMeteor)
      : wPtr(aPtr), mMainLoopPtr(mainLoopPtr), entitiesPtr(aEntities), meteorPtr(aMeteor) {
    SDL_Color white = {255, 255, 255}; 
    if (!(text = TTF_OpenFont("../thirdParty/LiberationSans-Regular.ttf", 18)) ) {
      text = TTF_OpenFont("thirdParty/LiberationSans-Regular.ttf", 18);
    }
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(text, "spacebar to resume    Esc to quit", white); 
    rect.x = 50;
    rect.y = 50;
    rect.w = 500;
    rect.h = 500;
    message = SDL_CreateTextureFromSurface(wPtr->renderer, surfaceMessage);
  }

  pauseState(const pauseState& other) = delete;
  pauseState& operator=(const pauseState& other) = delete;
  pauseState(pauseState&& other) noexcept = delete;
  pauseState& operator=(pauseState&& other) noexcept = delete;
  ~pauseState() override = {
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
  };
  void update(const double /*unused*/) override {}

  void processEvents() override {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            mMainLoopPtr->popPopState();
          break;
        case SDL_KeyboardEvent:
          if (event.key.keysym.sym == SDLK_SPACE) {
            mMainLoopPtr->popState();
          }
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            mMainLoopPtr->popPopState();
          }
          break;

        default:
          break;
      }
    }
  }

  void render(const double /*unused*/) override {
    //wPtr->clear(sf::Color::Black);
    //sf::RectangleShape greyBack;
    //greyBack.setFillColor(sf::Color(0, 0, 0, 200));
    //greyBack.setSize(wPtr->getView().getSize());
    //entitiesPtr->for_each([&](auto& ent) {
    //  if (ent.alive) {
    //    wPtr->draw(meteorPtr->getItem(ent.ID));
    //  }
    //});
    //wPtr->draw(greyBack);
    //wPtr->draw(text);
    //wPtr->display();

    //SDL_RenderClear(wPtr->renderer);
    //SDL_SetRenderDrawColor(wPtr->renderer, 255, 255, 255, 255);
    //SDL_RenderCopy(wPtr->renderer, message, NULL, &rect);
    //SDL_RenderPresent(wPtr->renderer);
  }

  SDLpointers mContext* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::entityHandler* entitiesPtr;
  kge::componentHandler<meteorShape>* meteorPtr;
  TTF_Font* const text;
  SDL_Texture* const message;
  SDL_Rect rect;
  SDL_Color white;
};

class gameState final : public kge::abstractState {
 public:
  gameState(SDLpointers mContext* const aPtr, kge::mainLoop* const mainLoopPtr, kge::threadPool<4>* const atpPtr)
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
      meteor.getItem(builtID).create();
    }
  }

  gameState(const gameState& other) = delete;
  gameState& operator=(const gameState& other) = delete;
  gameState(gameState&& other) noexcept = delete;
  gameState& operator=(gameState&& other) noexcept = delete;
  ~gameState() override = default;

  void update(const double dt) override { mUpdater.update(dt); }

  void processEvents() override {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            mMainLoopPtr->popState();
          break;
        case SDL_KeyboardEvent:
          if (event.key.keysym.sym == SDLK_SPACE) {
            mMainLoopPtr->pushState<pauseState, SDLpointers mContext* const, kge::mainLoop* const, kge::entityHandler*,
                                    kge::componentHandler<meteorShape>*>(wPtr, mMainLoopPtr, &entities, &meteor);
          }
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            mMainLoopPtr->popState();
          }
          break;
        case SDL_MouseButtonEvent:
          mSpawnSystem.spawnStack.emplace_back(event.x, event.y);
          break;
        default:
          break;
      }
    }
  }

  void render(const double dt) override {
    SDL_RenderClear(wPtr->renderer);
    mRenderSystem.update(dt);
    SDL_RenderPresent(wPtr->renderer);
  }

  SDLpointers mContext* const wPtr;
  kge::mainLoop* const mMainLoopPtr;
  kge::threadPool<4>* const mTpPtr;
  kge::entityHandler entities;
  kge::componentHandler<kge::point<double>> position;
  kge::componentHandler<bodyData> body;
  kge::componentHandler<meteorShape> meteor;
  movementSystem mMoveSystem;
  renderSystem mRenderSystem;
  spawnSystem mSpawnSystem;
  kge::updater<movementSystem, spawnSystem> mUpdater;
};

class menuState final : public kge::abstractState {
 public:
  menuState(SDLpointers mContext* const aPtr, kge::mainLoop* const mainLoopPtr, kge::threadPool<4>* const atpPtr)
      : wPtr(aPtr), mMainLoopPtr(mainLoopPtr), mTpPtr(atpPtr) {
    SDL_Color white = {255, 255, 255}; 
    if (!(text = TTF_OpenFont("../thirdParty/LiberationSans-Regular.ttf", 18)) ) {
      text = TTF_OpenFont("thirdParty/LiberationSans-Regular.ttf", 18);
    }
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(text, "thanks for trying kge,    spacebar to play/pause demo    Esc to quit", white); 
    rect.x = 50;
    rect.y = 50;
    rect.w = 500;
    rect.h = 500;
    message = SDL_CreateTextureFromSurface(wPtr->renderer, surfaceMessage);
  }

  menuState(const menuState& other) = delete;
  menuState& operator=(const menuState& other) = delete;
  menuState(menuState&& other) noexcept = delete;
  menuState& operator=(menuState&& other) noexcept = delete;
  ~menuState() override = {
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
  };

  void update(double /*unused*/) override {}

  void processEvents() override {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            mMainLoopPtr->popState();
          break;
        case SDL_KeyboardEvent:
          if (event.key.keysym.sym == SDLK_SPACE) {
            mMainLoopPtr
              ->switchState<gameState, SDLpointers mContext* const, kge::mainLoop* const, kge::threadPool<4>* const>(
                wPtr, mMainLoopPtr, mTpPtr);
          }
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            mMainLoopPtr->popState();
          }
          break;

        default:
          break;
      }
    }
  }

  void render(double /*unused*/) override {
    SDL_RenderClear(wPtr->renderer);
    SDL_SetRenderDrawColor(wPtr->renderer, 255, 255, 255, 255);
    SDL_RenderCopy(wPtr->renderer, message, NULL, &rect);
    SDL_RenderPresent(wPtr->renderer);
  }

  SDLpointers mContext* const wPtr;
  kge::mainLoop* mMainLoopPtr;
  kge::threadPool<4>* const mTpPtr;
  TTF_Font* const text;
  SDL_Texture* const message;
  SDL_Rect rect;
  SDL_Color white;

};

class SDLpointers {
	SDL_Renderer *renderer;
	SDL_Window *window;
}

int main() {
  SDLpointers mContext;
	int rendererFlags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC ;
	int windowFlags = 0;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error(std::string("SDL init failed: ") + SDL_GetError());
	}
  mContext.window = SDL_CreateWindow("kge example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, windowFlags);
  mContext.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

  kge::threadPool<4> mThreadPool;
  SDLpointers* const mPtr = &mWindow;
  kge::threadPool<4>* const tpPtr = &mThreadPool;
  kge::mainLoop myGame;
  kge::mainLoop* const gPtr = &myGame;
  myGame.pushState<menuState, SDLpointers* const, kge::mainLoop* const, kge::threadPool<4>* const>(mPtr, gPtr,
                                                                                                        tpPtr);
  myGame.run();

  return 0;
}

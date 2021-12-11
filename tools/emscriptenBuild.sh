#!/bin/bash
#docker run --rm -it -v /location/of/project/:/home/ emscripten/emsdk /bin/bash
em++ ./examples/sdl2Example.cpp -std=c++17 -pthread -O2 -gsource-map -s PTHREAD_POOL_SIZE=4 -s INITIAL_MEMORY=16777216 -s USE_SDL=2 -s USE_SDL_TTF=2 -o ./examples/index.js --preload-file ./thirdParty --source-map-base http://localhost:8080/ --emrun

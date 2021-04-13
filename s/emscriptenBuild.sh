#!/bin/bash
myDir=$PWD
em++ ./examples/sdl2Example.cpp -std=c++17 -O2 -gsource-map -s ASSERTIONS=1 -s INITIAL_MEMORY=16777216 -s USE_SDL=2 -s USE_SDL_TTF=2 -o ./examples/index.js --source-map-base http://localhost:8080/ --emrun

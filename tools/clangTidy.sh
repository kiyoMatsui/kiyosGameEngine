#!/bin/bash
myDir=$PWD
clang-tidy ./examples/sfmlExample.cpp -p ./build -header-filter=.* -- -I${PWD}/kge -std=c++17


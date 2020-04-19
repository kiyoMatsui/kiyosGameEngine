#!/bin/bash
myDir=$PWD
clang-tidy ./examples/sfmlExample.cpp -header-filter=.* -- -I${PWD}/kge -std=c++17


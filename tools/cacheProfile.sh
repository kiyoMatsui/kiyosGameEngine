#!/bin/bash
myDir=$PWD
valgrind --tool=cachegrind --branch-sim=yes --cache-sim=yes ./bin/sfmlExample
cg_annotate ./cachegrind.out.* ${PWD}/examples/sfmlExample.cpp #must be abolute path, no ./../. in path either this is a bit of a bug.

#!/bin/bash
myDir=$PWD
rm -r ./rtagsDir
rdm &
mkdir ./rtagsDir && cd ./rtagsDir
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=debug $myDir
rc -J

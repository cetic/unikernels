#!/bin/bash

docker run --rm -it -v "${PWD}":/usr/src/build -w /usr/src/build cetic/compiler g++ -std=c++11 -static -o runnableService *.cpp

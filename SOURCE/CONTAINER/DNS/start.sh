#!/bin/bash

# Building binary from C++ code (statically linking libraries)
docker run --rm -it -v "${PWD}":/usr/src/build -w /usr/src/build cetic/compiler g++ -std=c++11 -static -o runnableService *.cpp

# Building the container (expecting the Dockerfile to be present)
docker build -t cetic/dns .

# Launching the service in a Docker container
docker run --rm -d cetic/dns

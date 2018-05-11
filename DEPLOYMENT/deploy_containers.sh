#!/bin/bash
#NB: due to time constraint, only the DNS and WebServer analogs
#	have code ported from the Unikernel application.
#	Additionally the container ports are bound to the host's 
#	IP address for benchmarking.

SRC_DIR=$HOME/unikernels
CONTAINERS=$SRC_DIR/SOURCE/CONTAINER

# exposes the interface with internet connectivity for benchmarking
# if your benchmarking device is on another network, adjust accordingly
IP=$(ip -4 route get 8.8.8.8 | awk {'print $7'} | tr -d '\n')
DNS_PORT=53
WEB_PORT=80

# build Docker containers
# 1 - compiler docker
pushd $CONTAINERS &&
docker build -t cetic/compiler . &&
popd &&

# 2 - individual dockers
pushd $CONTAINERS/DNS &&
docker run --rm -it -v "${PWD}":/usr/src/build -w /usr/src/build cetic/compiler g++ -std=c++11 -static -o runnableService *.cpp &&
docker build -t cetic/dns . &&
popd &&

pushd $CONTAINERS/WebServer &&
docker run --rm -it -v "${PWD}":/usr/src/build -w /usr/src/build cetic/compiler g++ -std=c++11 -static -o runnableService *.cpp &&
docker build -t cetic/webserver . &&
popd &&

# launch docker containers
docker run --rm --cpuset-cpus 1 -m 128m -d -p $IP:$DNS_PORT:$DNS_PORT/udp --name DNS cetic/dns
docker run --rm --cpuset-cpus 1 -m 128m -d -p $IP:$WEB_PORT:$WEB_PORT --name WebServer cetic/webserver

echo "#############################################################################"
echo "###                                                                       ###"
echo "###                   Containers successfully launched.                   ###"
echo "###                   Web server accessible on port $WEB_PORT.                   ###"
echo "###                   DNS server accessible on port $DNS_PORT.                   ###"
echo "###                                                                       ###"
echo "#############################################################################"

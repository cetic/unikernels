#!/bin/bash
SRC_DIR=$HOME/unikernels
CONTAINERS=$SRC_DIR/SOURCE/CONTAINER

#IP=$(ip -4 route get 8.8.8.8 | awk {'print $7'} | tr -d '\n')
IP=10.65.0.101
PORT=53

source $SRC_DIR/BENCHMARKING/bench_unikernel_cleanup.sh

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

# stopping all VMs and containers
docker stop $(docker ps -a -q)
virsh list | grep running | awk '{ print $2}' | while read DOMAIN; do
        virsh destroy $DOMAIN
        sleep 3
done

# launch docker containers
docker run --rm --cpus 1 -m 128m -d -p $IP:$PORT:$PORT/udp --name DNSBench cetic/dns &&

echo "#############################################################################" &&
echo "###                                                                       ###" &&
echo "###               Container DNS Server ready for benchmark.               ###" &&
echo "###                           $IP:$PORT                            ###" &&
echo "###                                                                       ###" &&
echo "#############################################################################"

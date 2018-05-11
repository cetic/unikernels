#!/bin/bash

SRC_DIR=$HOME/unikernels/BENCHMARKING

# DNSPerf
sudo apt-get install -y libbind-dev libkrb5-dev libssl-dev libcap-dev libxml2-dev libgeoip-dev bind9utils make &&
wget ftp://ftp.nominum.com/pub/nominum/dnsperf/2.1.0.0/dnsperf-src-2.1.0.0-1.tar.gz &&
tar xzvf dnsperf-src-2.1.0.0-1.tar.gz &&
pushd dnsperf-src-2.1.0.0-1 &&
./configure &&
make &&
sudo make install &&
popd &&
wget ftp://ftp.nominum.com/pub/nominum/dnsperf/data/queryfile-example-current.gz &&
gunzip queryfile-example-current.gz &&
#Usage: dnsperf -s 10.0.0.100 -d queryfile-example-current -l 60 -c 1 -Q 10

#wrk2 install
git clone https://github.com/giltene/wrk2 &&
pushd wrk2 &&
sudo make &&
sudo cp wrk /usr/local/bin/wrk2 &&
popd
#Usage: wrk2 -d 5m -R 100 http://10.0.0.5/

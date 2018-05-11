#!/bin/bash
SRC_DIR=$HOME/unikernels/BENCHMARKING
UNIKERNELS=$SRC_DIR/UNIKERNEL

HOST_IP=$(ip -4 route get 8.8.8.8 | awk {'print $7'} | tr -d '\n')
GUEST_IP=192.168.122.5
GUEST_PORT=80
HOST_PORT=80
BRIDGE_INTERFACE=virbr0

# writing correct JSON file
pushd $UNIKERNELS/DNS &&
jq -n --arg IP_ADDRESS "$GUEST_IP" '{"net": [{"netmask": "255.255.255.0", "config": "static", "iface": 0, "gateway": "192.168.122.1", "address": $IP_ADDRESS}]}' > config.json &&
# build unikernels
sudo rm -rf build &&
mkdir -p build &&
pushd build &&
cmake .. &&
sudo make &&
popd &&
popd &&

# shuting down all VMs
docker stop $(docker ps -a -q)
virsh list | grep running | awk '{ print $2}' | while read DOMAIN; do
	virsh destroy $DOMAIN
	sleep 3
done
virsh undefine WebServerBench

# launch unikernels in QEMU (+serial output to files)
virt-install \
	--virt-type qemu --name WebServerBench \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERNELS/WebServer/build/WebServer.img \
	--serial file,path=/tmp/web-benchmark.log \
	--network network=default,model=virtio \
	--check path_in_use=off \
	--noautoconsole &&

# enable port-forwarding through iptables
source $SRC_DIR/bench_unikernel_cleanup.sh
sudo iptables -I FORWARD -o $BRIDGE_INTERFACE -d $GUEST_IP -j ACCEPT &&
sudo iptables -t nat -I PREROUTING -p tcp --dport $HOST_PORT -j DNAT --to $GUEST_IP:$GUEST_PORT &&

echo "#############################################################################" &&
echo "###                                                                       ###" &&
echo "###               Unikernel Web Server ready for benchmark.               ###" &&
echo "###                           $HOST_IP:$HOST_PORT                            ###" &&
echo "###                                                                       ###" &&
echo "#############################################################################"


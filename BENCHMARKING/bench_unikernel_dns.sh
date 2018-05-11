#!/bin/bash
SRC_DIR=$HOME/unikernels/BENCHMARKING
UNIKERNELS=$SRC_DIR/UNIKERNEL

HOST_IP=$(ip -4 route get 8.8.8.8 | awk {'print $7'} | tr -d '\n')
GUEST_IP=192.168.122.100
GUEST_PORT=53
HOST_PORT=53
BRIDGE_INTERFACE=virbr0

#write correct JSON file
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
        if [ "$DOMAIN" == "DNSBench" ]
	then
                virsh undefine $DOMAIN
        fi
done
virsh undefine DNSBench

# launch unikernels in QEMU (+serial output to files)
virt-install \
	--virt-type qemu --name DNSBench \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERNELS/DNS/build/DNS.img \
	--serial file,path=/tmp/dns-benchmark.log \
	--network network=default,model=virtio \
	--check path_in_use=off \
	--noautoconsole &&

# enable port-forwarding through iptables
source $SRC_DIR/bench_unikernel_cleanup.sh
sudo iptables -I FORWARD -o $BRIDGE_INTERFACE -d $GUEST_IP -j ACCEPT &&
sudo iptables -t nat -I PREROUTING -p udp --dport $HOST_PORT -j DNAT --to $GUEST_IP:$GUEST_PORT &&

echo "#############################################################################" &&
echo "###                                                                       ###" &&
echo "###               Unikernel DNS Server ready for benchmark.               ###" &&
echo "###                           $HOST_IP:$HOST_PORT                            ###" &&
echo "###                                                                       ###" &&
echo "#############################################################################"

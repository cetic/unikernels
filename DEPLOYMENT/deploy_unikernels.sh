#!/bin/bash

# NB: This script assumes you cloned the project into you HOME directory.
#		If this is not the case, change the following variables accordingly

SRC_DIR=$HOME/unikernels
UNIKERELS=$SRC_DIR/SOURCE/UNIKERNEL

# build unikernels
pushd $UNIKERELS/DNS &&\
mkdir -p build &&\
pushd build &&\
cmake .. &&\
make &&\
popd &&\

pushd $UNIKERELS/WebServer &&\
mkdir -p build &&\
pushd build &&\
cmake .. &&\
sudo make &&\
popd &&\

pushd $UNIKERELS/Router &&\
mkdir -p build &&\
pushd build &&\
cmake .. &&\
make &&\
popd &&\

pushd $UNIKERELS/Firewall &&\
mkdir -p build &&\
pushd build &&\
cmake .. &&\
make &&\
popd &&\

# configure networks for QEMU
# remove default bridge created
sudo ip link del bridge43 
# create the bridge interfaces
virsh net-define --file $UNIKERELS/internal.xml &&
virsh net-define --file $UNIKERELS/external.xml &&
virsh net-define --file $UNIKERELS/firewall.xml &&
virsh net-start internal &&
virsh net-start external &&
virsh net-start firewall &&
virsh net-autostart internal &&
virsh net-autostart external &&
virsh net-autostart firewall &&

# launch unikernels in QEMU (+serial output to files)

virt-install \
	--virt-type qemu --name DNS \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERELS/DNS/build/DNS.img \
	--serial file,path=/tmp/dns.log \
	--network network=internal,model=virtio \
	--noautoconsole &&

virt-install \
	--virt-type qemu --name WebServer \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERELS/WebServer/build/WebServer.img \
	--serial file,path=/tmp/web.log \
	--network network=internal,model=virtio \
	--noautoconsole &&

virt-install \
	--virt-type qemu --name Router \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERELS/Router/build/Router.img \
	--serial file,path=/tmp/router.log \
	--network network=internal,model=virtio \
	--network network=firewall,model=virtio \
	--noautoconsole &&

virt-install \
	--virt-type qemu --name Firewall \
	--vcpus 1 --ram 128 \
	--import --disk $UNIKERELS/Firewall/build/Firewall.img \
	--serial file,path=/tmp/firewall.log \
	--network network=external,model=virtio \
	--network network=firewall,model=virtio \
	--noautoconsole &&

echo "#############################################################################"
echo "###                                                                       ###"
echo "###                   Unikernels successfully launched.                   ###"
echo "###                  Serial outputs are located in /tmp.                  ###"
echo "###                                                                       ###"
echo "#############################################################################"

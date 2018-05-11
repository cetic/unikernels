#!/bin/bash

# REMOVING IP TABLES RULES FOR UNIKERNEL BENCHMARKING
WEB_IP=192.168.122.5
DNS_IP=192.168.122.10
WEB_PORT=80
DNS_PORT=53
BRIDGE_INTERFACE=virbr0

sudo iptables -D FORWARD -o $BRIDGE_INTERFACE -d $DNS_IP -j ACCEPT
sudo iptables -t nat -D PREROUTING -p udp --dport $DNS_PORT -j DNAT --to $DNS_IP:$DNS_PORT
sudo iptables -D FORWARD -o $BRIDGE_INTERFACE -d $WEB_IP -j ACCEPT
sudo iptables -t nat -D PREROUTING -p tcp --dport $WEB_PORT -j DNAT --to $WEB_IP:$WEB_PORT

echo "This will stop and delete all VMs and Containers !"
read -p "Are you sure ? [Y|N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
	# STOPPING AND DELETING ALL CONTAINERS
	docker stop $(docker ps -a -q)
	docker rm $(docker ps -a -q)
	virsh list | grep running | awk '{ print $2}' | while read DOMAIN; do
		virsh destroy $DOMAIN
	done
	virsh list --all | grep "shut off" | awk '{ print $2}' | while read DOMAIN; do
	        virsh undefine $DOMAIN
	done
fi


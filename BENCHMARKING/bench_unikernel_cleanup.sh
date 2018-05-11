#!/bin/bash

WEB_IP=192.168.122.5
DNS_IP=192.168.122.100
WEB_PORT=80
DNS_PORT=53
BRIDGE_INTERFACE=virbr0

sudo iptables -D FORWARD -o $BRIDGE_INTERFACE -d $DNS_IP -j ACCEPT
sudo iptables -t nat -D PREROUTING -p udp --dport $DNS_PORT -j DNAT --to $DNS_IP:$DNS_PORT

sudo iptables -D FORWARD -o $BRIDGE_INTERFACE -d $WEB_IP -j ACCEPT
sudo iptables -t nat -D PREROUTING -p tcp --dport $WEB_PORT -j DNAT --to $WEB_IP:$WEB_PORT

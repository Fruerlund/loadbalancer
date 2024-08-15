#!/bin/bash
# Create network namespaces for our loadserver and webservers.
sudo ip netns add loadserver
sudo ip netns exec loadserver ip link set lo up
sudo ip netns add webserver01
sudo ip netns exec webserver01 ip link set lo up
sudo ip netns add webserver02
sudo ip netns exec webserver02 ip link set lo up

# Create a bridge.
sudo ip link add br0 type bridge
sudo ip link set br0 up
sudo ip addr add 192.168.1.1/24 dev br0

# Create a VIRTUAL Ethernet Pair for each namespace and attach one end to the namespace and one end to the switch.
sudo ip link add veth0 type veth peer name ceth0
sudo ip link set veth0 master br0
sudo ip link set veth0 up
sudo ip link set ceth0 netns loadserver
sudo ip netns exec loadserver ip link set ceth0 up
sudo ip netns exec loadserver ip addr add 192.168.1.100/24 dev ceth0

sudo ip link add veth1 type veth peer name ceth1
sudo ip link set veth1 master br0
sudo ip link set veth1 up
sudo ip link set ceth1 netns webserver01
sudo ip netns exec webserver01 ip link set ceth1 up
sudo ip netns exec webserver01 ip addr add 192.168.1.101/24 dev ceth1

sudo ip link add veth2 type veth peer name ceth2
sudo ip link set veth2 master br0
sudo ip link set veth2 up
sudo ip link set ceth2 netns webserver02
sudo ip netns exec webserver02 ip link set ceth2 up
sudo ip netns exec webserver02 ip addr add 192.168.1.102/24 dev ceth2

# Create a virtual interface on the loadserver to disable network stack.
sudo ip netns exec loadserver ip link add link ceth0 address 00:11:11:11:11:11 virtual0 type macvlan
sudo ip netns exec loadserver ip link set ceth0 promisc on
sudo ip netns exec loadserver ip link set virtual0 up

sudo ip netns exec webserver01 ifconfig lo:1 192.168.1.200 netmask 255.255.255.255 -arp up
sudo ip netns exec webserver02 ifconfig lo:1 192.168.1.200 netmask 255.255.255.255 -arp up

# Disable ARP Replies for virtual IP address on webservers.
sudo ip netns exec webserver01 sysctl -w net.ipv4.conf.lo.arp_ignore=0
sudo ip netns exec webserver01 sysctl -w net.ipv4.conf.lo.arp_announce=0
sudo ip netns exec webserver01 sysctl -w net.ipv4.conf.all.arp_ignore=0
sudo ip netns exec webserver01 sysctl -w net.ipv4.conf.all.arp_announce=0

sudo ip netns exec webserver02 sysctl -w net.ipv4.conf.lo.arp_ignore=1
sudo ip netns exec webserver02 sysctl -w net.ipv4.conf.lo.arp_announce=2
sudo ip netns exec webserver02 sysctl -w net.ipv4.conf.all.arp_ignore=1
sudo ip netns exec webserver02 sysctl -w net.ipv4.conf.all.arp_announce=2



#!/usr/bin/env sh

# usage:
# ./prepare_eth_interface eno1

IFACE=$1

#do not expect to get an internet connection
nmcli device set $IFACE managed no

#stop sending a bunch of ipv6 packets
sudo sysctl -w net.ipv6.conf.$IFACE.disable_ipv6=1

#disable autonegotiation and force 100BaseT/Full
#this is not possible on all network devices (eg usb ethernet adapters)
# sudo ethtool -s $IFACE speed 100 duplex full autoneg off

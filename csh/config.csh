#!/usr/bin/env csh

#The ethernet interface connected to ASTRA-LEO
#List available interfaces with "ip a" in the terminal
var set ETH_IFACE "eno1"

# The CSP address of the ethernet interface in the device running csh 
# This can be pretty much any value as long as it is within the same subnet
var set HOST_CSP_ADDR 200


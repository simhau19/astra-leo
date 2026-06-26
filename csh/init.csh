#!/usr/bin/env csh

run $(__FILE_DIR__)/config.csh
run $(__FILE_DIR__)/astra-leo.csh

csp add eth -d $(HOST_CSP_ADDR) $(ETH_IFACE)

#ifndef CSP_ETH_SAM_H
#define CSP_ETH_SAM_H

#include "hal_mac_async.h"
#include <csp/csp.h>

int csp_eth_init(const struct mac_async_descriptor* gmac_handle, const char* ifname, int mtu, int node_id, bool promisc, csp_iface_t** return_iface);

#endif //CSP_ETH_SAM_H

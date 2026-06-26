#include "aldp.h"

#include "app_config.h"

#include <FreeRTOSConfig.h>
#include <csp/csp.h>
#include <csp/csp_debug.h>
#include <csp/csp_types.h>
#include <csp/drivers/eth_linux.h>

#include <FreeRTOS.h>

#include <argp.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE_SERVER 256

static struct argp_option options[] = {
    {"interface", 'e', "STRING", 0, "Set the ethernet interface (eg. eth1)", 0},
    {0}};

struct arguments {
    char* interface;
};

/* Parse a single option/argument */
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;

    switch (key) {
    case 'e':
        arguments->interface = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, "", "Dummy Payload Controller for ASTRA-LEO", NULL, NULL, NULL};

void print_aldp_header(ALDPHeader header) {
    printf("ALDP version: %d \r\n", header.version);
    printf("ALDP ASTRA-LEO ID: %d \r\n", header.al_id);
    printf("ALDP type: %d \r\n", header.packet_type);
}

void print_aldp_packet(uint8_t* buf, ALDPPacketType type) {
    switch (type) {
    case ALDP_PACKET_ADVANCED: {
        ALDPAdvanced packet;
        memcpy(&packet, buf, sizeof(packet));
        printf("Advanced packet:\r\n");
        printf("Crystal energies:\r\n");
        for (size_t i = 0; i < sizeof(packet.crystal_energy) / sizeof(packet.crystal_energy[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.crystal_energy[i]);
        }
        printf("Plastic energies:\r\n");
        for (size_t i = 0; sizeof(packet.plastic_energy) / sizeof(packet.plastic_energy[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.plastic_energy[i]);
        }
        printf("Temperatures:\r\n");
        for (size_t i = 0; i < sizeof(packet.temperature) / sizeof(packet.temperature[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.temperature[i]);
        }
        printf("Timestamp:\r\n\t %d\r\n", packet.timestamp);
        break;
    }
    case ALDP_PACKET_COUNT: {
        ALDPCount packet;
        memcpy(&packet, buf, sizeof(packet));
        printf("Count packet:\r\n");
        printf("Crystal energies:\r\n");
        for (size_t i = 0; i < sizeof(packet.crystal_count) / sizeof(packet.crystal_count[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.crystal_count[i]);
        }
        printf("Plastic energies:\r\n");
        for (size_t i = 0; sizeof(packet.plastic_count) / sizeof(packet.plastic_count[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.plastic_count[i]);
        }
        printf("Temperatures:\r\n");
        for (size_t i = 0; i < sizeof(packet.temperature) / sizeof(packet.temperature[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.temperature[i]);
        }
        printf("Timestamp:\r\n\t%d\r\n", packet.timestamp);
        break;
    }
    case ALDP_PACKET_CRYSTAL_EVENT: {
        ALDPCrystalEvent packet;
        memcpy(&packet, buf, sizeof(packet));
        printf("Crystal event packet:\r\n");
        printf("Crystal energies:\r\n");
        for (size_t i = 0; i < sizeof(packet.crystal_energy) / sizeof(packet.crystal_energy[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.crystal_energy[i]);
        }
        printf("Plastic trigger values:\r\n\t 0X%X\r\n", packet.plastic_trigger);
        printf("Temperatures:\r\n");
        for (size_t i = 0; i < sizeof(packet.temperature) / sizeof(packet.temperature[0]); i++) {
            printf("\t[%zu]: %d\r\n", i, packet.temperature[i]);
        }
        printf("Timestamp:\r\n\t %d\r\n", packet.timestamp);
        break;
    }
    case ALDP_PACKET_GAMMA: {
        ALDPGamma packet;
        memcpy(&packet, buf, sizeof(packet));
        printf("Gamma packet:\r\n");
        printf("Energy: %d\r\n", packet.energy);
        printf("Timestamp:\r\n\t %d\r\n", packet.timestamp);
        break;
    }
    case ALDP_PACKET_NEUTRON: {
        ALDPNeutron packet;
        memcpy(&packet, buf, sizeof(packet));
        printf("Neutron packet:\r\n");
        printf("Energy: %d\r\n", packet.energy);
        printf("Timestamp:\r\n\t %d\r\n", packet.timestamp);
        break;
    }
    case ALDP_PACKET_NONE:
        printf("No packet type\r\n");
        break;
    default:
        printf("Unknown packet type\r\n");
        break;
    }
}

void csp_router(void* param) {
    (void)param;
    csp_print("Routing task started\r\n");
    for (;;) {
        csp_route_work();
    }
}

void server(void* param) {
    (void)param;
    printf("Server task started.\r\n");

    csp_socket_t socket = {0};

    csp_bind(&socket, CSP_ANY);
    printf("socket binded\r\n");

    csp_listen(&socket, 10);
    printf("socket listening \r\n");

    csp_conn_t* conn;
    while ((conn = csp_accept(&socket, CSP_MAX_TIMEOUT)) == NULL) {
        printf("connection failed\r\n");
    }

    printf("connection established\r\n");

    csp_packet_t* packet;

    for (;;) {
        packet = csp_read(conn, CSP_MAX_TIMEOUT);
        if (packet == NULL) {
            csp_print("Connection error. attempting to reconnect\r\n");
            csp_close(conn);
            conn = csp_accept(&socket, CSP_MAX_TIMEOUT);
            continue;
        }

        printf("Packet received:\r\n");
        if (packet->length < sizeof(ALDPHeader)) {
            printf("Unexpectedly short packet \r\n");
            csp_buffer_free(packet);
        }

        ALDPHeader aldp_header;
        memcpy(&aldp_header, packet->data, sizeof(ALDPHeader));
        bool valid;
        valid                = aldp_header.version == 0;
        size_t required_size = sizeof(ALDPHeader);
        switch (aldp_header.packet_type) {
        case ALDP_PACKET_ADVANCED:
            required_size += sizeof(ALDPAdvanced);
            break;
        case ALDP_PACKET_COUNT:
            required_size += sizeof(ALDPCount);
            break;
        case ALDP_PACKET_GAMMA:
            required_size += sizeof(ALDPGamma);
            break;
        case ALDP_PACKET_NEUTRON:
            required_size += sizeof(ALDPNeutron);
            break;
        case ALDP_PACKET_CRYSTAL_EVENT:
            required_size += sizeof(ALDPCrystalEvent);
            break;
        case ALDP_PACKET_NONE:
        default:
            valid = 0;
            break;
        }

        valid = valid && packet->length >= required_size;
        if (!valid) {
            printf("Invalid packet\r\n");
            print_aldp_header(aldp_header);
            printf("Expected length of %zu. Got buffer of size %d", required_size, packet->length);
        } else {
            print_aldp_packet(packet->data + sizeof(ALDPHeader), aldp_header.packet_type);
        }

        csp_buffer_free(packet);
    }
}

int main(int argc, char* argv[]) {

    struct arguments args = {NULL};

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (args.interface == NULL) {
        printf("No interface specified\r\n");
        return 1;
    }

    xTraceInitialize();
    xTraceEnable(TRC_START);

    csp_init();

    csp_iface_t* eth_iface;
    const char*  csp_eth_if_name = "eth";
    int          eth_mtu         = 1500;
    uint16_t     eth_if_addr     = CSP_ADDR_SDR0;

    int status;
    status = csp_eth_init(args.interface, csp_eth_if_name, eth_mtu, eth_if_addr, true, &eth_iface);
    if (status != CSP_ERR_NONE) {
        printf("csp_eth_init returned with error code %d. terminating\r\n", status);
        return 1;
    }
    eth_iface->is_default = true;

    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    static StaticTask_t csp_router_tcb;
    static StackType_t  csp_router_stack[STACK_SIZE_CSP_ROUTER];
    xTaskCreateStatic(csp_router,
                      "router",
                      STACK_SIZE_CSP_ROUTER,
                      NULL,
                      tskIDLE_PRIORITY + 2,
                      csp_router_stack,
                      &csp_router_tcb);

    static StaticTask_t server_tcb;
    static StackType_t  server_stack[STACK_SIZE_SERVER];
    xTaskCreateStatic(server,
                      "Server",
                      STACK_SIZE_SERVER,
                      NULL,
                      tskIDLE_PRIORITY + 2,
                      server_stack,
                      &server_tcb);

    vTaskStartScheduler();
}

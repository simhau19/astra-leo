#include "data_handler.h"
#include "readout.h"

#include "aldp.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "csp/csp_types.h"
#include "siphra.h"
#include <csp/csp.h>
#include <csp/csp_debug.h>

size_t get_packet_size(ALDPPacketType type) {
    switch (type) {
    case ALDP_PACKET_ADVANCED:
        return sizeof(ALDPHeader) + sizeof(ALDPAdvanced);
    case ALDP_PACKET_COUNT:
        return sizeof(ALDPHeader) + sizeof(ALDPCount);
    case ALDP_PACKET_CRYSTAL_EVENT:
        return sizeof(ALDPHeader) + sizeof(ALDPCrystalEvent);
    case ALDP_PACKET_GAMMA:
        return sizeof(ALDPHeader) + sizeof(ALDPGamma);
    case ALDP_PACKET_NEUTRON:
        return sizeof(ALDPHeader) + sizeof(ALDPNeutron);
    case ALDP_PACKET_NONE:
        return sizeof(ALDPHeader);
    default:
        return 0;
    }
}

void connect_to_server(csp_conn_t** conn, uint16_t addr, uint8_t port) {

    *conn = NULL;

    while (*conn == NULL) {

        *conn = csp_connect(
            CSP_PRIO_NORM,
            addr,
            port,
            1000, // timeout (not used)
            CSP_O_CRC32 | CSP_O_RDP);

        if (*conn != NULL)
            csp_print("Connection established!\r\n");
    }
}

void data_handler_task(void* param) {
    printf("data handler task started \r\n");

    DataHandlerArgs* args = (DataHandlerArgs*)param;

    printf("initiating csp connection \r\n");

    csp_conn_t* conn = NULL;

    connect_to_server(&conn, args->server_addr, args->server_port);

    for (;;) {

        BaseType_t     status;
        DataHandlerMsg data;
        status = xQueueReceive(args->input_queue, &data, portMAX_DELAY);

        if (!csp_conn_is_active(conn)) {
            csp_close(conn);
            connect_to_server(&conn, args->server_addr, args->server_port);
        }

        if (status != pdPASS)
            continue;

        csp_packet_t* csp_packet = csp_buffer_get(0);

        if (csp_packet == NULL) {
            csp_print("Failed to get buffer\r\n");
            csp_close(conn);
            connect_to_server(&conn, args->server_addr, args->server_port);
            continue;
        }

        ALDPPacket  aldp_packet;
        ALDPHeader* aldp_header  = &aldp_packet.header;
        aldp_header->al_id       = 0;
        aldp_header->version     = 0;
        aldp_header->packet_type = ALDP_PACKET_NONE;

        if (data.type == DATA_HANDLER_MESSAGE_TYPE_SPECTROSCOPY) {

            if (data.spectroscopy_data.plastic_spectroscopy_type == SIPHRA_PLASTIC_SPECTROSCOPY_FLAGS) {
                aldp_header->packet_type = ALDP_PACKET_CRYSTAL_EVENT;
                memcpy(aldp_packet.crystal.crystal_energy, data.spectroscopy_data.crystal_energy, sizeof(data.spectroscopy_data.crystal_energy));
                memcpy(aldp_packet.crystal.temperature, data.temp_probes, sizeof(data.temp_probes));
                aldp_packet.crystal.plastic_trigger = data.spectroscopy_data.plastic_detection_flags & 0x3F;
            } else if (data.spectroscopy_data.plastic_spectroscopy_type == SIPHRA_PLASTIC_SPECTROSCOPY_FULL) {
                aldp_header->packet_type = ALDP_PACKET_ADVANCED;
                memcpy(aldp_packet.advanced.crystal_energy, data.spectroscopy_data.crystal_energy, sizeof(data.spectroscopy_data.crystal_energy));
                memcpy(aldp_packet.advanced.plastic_energy, data.spectroscopy_data.plastic_energy, sizeof(data.spectroscopy_data.plastic_energy));
            }
        } else if (data.type == DATA_HANDLER_MESSAGE_TYPE_COUNT) {
            aldp_header->packet_type = ALDP_PACKET_COUNT;
            memcpy(aldp_packet.count.crystal_count, data.count.crystal, sizeof(data.count.crystal));
            memcpy(aldp_packet.count.plastic_count, data.count.plastic, sizeof(data.count.plastic));
        }

        csp_packet->length = (uint16_t)get_packet_size(aldp_header->packet_type);
        memcpy(csp_packet->data, &aldp_packet, csp_packet->length);
        // memcpy(&csp_packet->data[sizeof(ALDPHeader)], &data, sizeof(data));

        csp_send(conn, csp_packet);
    }
}

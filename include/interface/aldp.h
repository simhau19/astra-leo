#ifndef ALDP_H
#define ALDP_H

#include <stdint.h>

typedef enum {
    ALDP_PACKET_NONE          = 0,
    ALDP_PACKET_COUNT         = 1,
    ALDP_PACKET_CRYSTAL_EVENT = 2,
    ALDP_PACKET_ADVANCED      = 3,
    ALDP_PACKET_GAMMA         = 4,
    ALDP_PACKET_NEUTRON       = 5,
} ALDPPacketType;

typedef struct {
    uint16_t version     : 4;
    uint16_t packet_type : 4;
    uint16_t             : 4;
    uint16_t al_id       : 4;
} ALDPHeader;

typedef struct {
    uint32_t timestamp;
    uint32_t crystal_count[4];
    uint32_t plastic_count[6];
    uint8_t  temperature[16];
} ALDPCount;

typedef struct {
    uint32_t timestamp;
    uint16_t crystal_energy[4];
    uint8_t  plastic_trigger : 6;
    uint8_t  forced_readout  : 1;
    uint8_t                  : 1;
    uint8_t temperature[16];
} ALDPCrystalEvent;

typedef struct {
    uint32_t timestamp;
    uint16_t crystal_energy[4];
    uint16_t plastic_energy[6];
    uint8_t  temperature[16];
} ALDPAdvanced;

typedef struct {
    uint32_t timestamp;
    uint32_t energy;
} ALDPGamma;

typedef struct {
    uint32_t timestamp;
    uint32_t energy;
} ALDPNeutron;

typedef struct __attribute__((packed)) {
    ALDPHeader header;
    union {
        ALDPCount        count;
        ALDPCrystalEvent crystal;
        ALDPAdvanced     advanced;
        ALDPGamma        gamma;
        ALDPNeutron      neutron;
    };
} ALDPPacket;

#endif

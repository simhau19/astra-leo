#ifndef SIPHRA_H
#define SIPHRA_H

#include <stdbool.h>
#include <stdint.h>

#define NUM_SIPHRAS          6
#define NUM_SIPHRA_REGISTERS 0x21

typedef struct {
    uint8_t  ic_id;
    uint8_t  addr;
    uint32_t value;
} SiphraRegister;

typedef struct {
    uint32_t crystal[4];
    uint32_t plastic[6];
} SiphraDetectionCount;

typedef enum {
    SIPHRA_PLASTIC_SPECTROSCOPY_NONE  = 0,
    SIPHRA_PLASTIC_SPECTROSCOPY_FLAGS = 1,
    SIPHRA_PLASTIC_SPECTROSCOPY_FULL  = 2,
} SiphraPlasticSpectroscopyType;

typedef struct {
    uint16_t                      crystal_energy[4];
    SiphraPlasticSpectroscopyType plastic_spectroscopy_type;
    uint8_t                       plastic_detection_flags;
    uint16_t                      plastic_energy[6];
} SiphraSpectroscopy;

void siphra_read_register(SiphraRegister* reg);
void siphra_write_register(SiphraRegister* reg);

void siphra_do_readout(SiphraSpectroscopy* data);

void siphra_get_detection_count(SiphraDetectionCount* cnt);
void siphra_reset_detection_count(void);

#endif // SIPHRA_H

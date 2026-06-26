#include <siphra.h>

#include <FreeRTOS.h>
#include <semphr.h>

static uint32_t registers[NUM_SIPHRAS][NUM_SIPHRA_REGISTERS] = {{0}};

static SemaphoreHandle_t _get_mtx(void) {
    static SemaphoreHandle_t mtx = NULL;
    static StaticSemaphore_t mtx_buffer;
    if (mtx == NULL) {
        mtx = xSemaphoreCreateMutexStatic(&mtx_buffer);
    }
    return mtx;
}

void siphra_read_register(SiphraRegister* reg) {
    if (reg->ic_id < NUM_SIPHRAS && reg->addr < NUM_SIPHRA_REGISTERS) {
        if (xSemaphoreTake(_get_mtx(), portMAX_DELAY) != pdTRUE) {
            return;
        }
        reg->value = registers[reg->ic_id][reg->addr];
        xSemaphoreGive(_get_mtx());
    }
}
void siphra_write_register(SiphraRegister* reg) {
    if (reg->ic_id < NUM_SIPHRAS && reg->addr < NUM_SIPHRA_REGISTERS) {
        if (xSemaphoreTake(_get_mtx(), portMAX_DELAY) != pdTRUE) {
            return;
        }
        registers[reg->ic_id][reg->addr] = reg->value;
        xSemaphoreGive(_get_mtx());
    }
}

void siphra_do_readout(SiphraSpectroscopy* data) {
    data->crystal_energy[0] = (uint16_t)rand();
    data->crystal_energy[1] = (uint16_t)rand();
    data->crystal_energy[2] = (uint16_t)rand();
    data->crystal_energy[3] = (uint16_t)rand();

    // check what type of data we are getting from the siphra (only flags or full plastic spectroscopy)
    data->plastic_spectroscopy_type = SIPHRA_PLASTIC_SPECTROSCOPY_FLAGS;
    data->plastic_detection_flags   = (uint8_t)rand() & 0x3F;
}

void siphra_get_detection_count(SiphraDetectionCount* cnt) {
    (void)cnt;
    void siphra_get_detection_count(SiphraDetectionCount * cnt);
    void siphra_get_detection_count(SiphraDetectionCount * cnt);
}

void siphra_reset_detection_count(void) {}

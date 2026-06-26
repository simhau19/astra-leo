#include "siphra.h"

#include <param/param.h>
#include <param_config.h>
#include <stdint.h>
#include <vmem/vmem.h>
#include <vmem/vmem_mmap.h>

#define _SIPHRA_ID_BP  16
#define _SIPHRA_ID_BM  (0xF << _SIPHRA_ID_BP)
#define _SIPHRA0_START (0x0 << _SIPHRA_ID_BP)
#define _SIPHRA1_START (0x1 << _SIPHRA_ID_BP)
#define _SIPHRA2_START (0x2 << _SIPHRA_ID_BP)
#define _SIPHRA3_START (0x3 << _SIPHRA_ID_BP)
#define _SIPHRA4_START (0x4 << _SIPHRA_ID_BP)
#define _SIPHRA5_START (0x5 << _SIPHRA_ID_BP)

#define _SIPHRA_VMEM_SIZE (0x6 << _SIPHRA_ID_BP)

// the virtual address for writing to the siphras consists of both the siphra id and the
// physical register address
// The virtual address 0x30008 corresponds to siphra3's register 0x08

static uint8_t get_siphra_id_from_vaddr(uint64_t vaddr) {
    return (vaddr & _SIPHRA_ID_BM) >> _SIPHRA_ID_BP;
}

static uint8_t get_siphra_reg_addr_from_vaddr(uint64_t vaddr) {
    // vaddr is byte addressed while the siphra registers are 4-byte addressed
    return (vaddr & 0xFF) / 4;
}

void vmem_siphra_read(const vmem_t* vmem, uint64_t addr, void* dataout, uint32_t len) {
    // should it return (do nothing) if not in DEBUG mode?

    SiphraRegister reg;
    uint32_t*      reg_values = (uint32_t*)dataout;
    reg.ic_id                 = get_siphra_id_from_vaddr(addr);
    for (int i = 0; i < len; i += 4) {
        reg.addr  = get_siphra_reg_addr_from_vaddr(addr + (i*4));
        siphra_read_register(&reg);
        reg_values[i] = reg.value;
    }
}
void vmem_siphra_write(const vmem_t* vmem, uint64_t addr, const void* dataout, uint32_t len) {
    // should it return (do nothing) if not in DEBUG mode?

    SiphraRegister reg;
    uint32_t*      reg_values = (uint32_t*)dataout;
    reg.ic_id                 = get_siphra_id_from_vaddr(addr);
    for (int i = 0; i < len / 4; i++) {
        reg.addr  = get_siphra_reg_addr_from_vaddr(addr + (i * 4));
        reg.value = reg_values[i];
        siphra_write_register(&reg);
    }
}

vmem_t vmem_siphra = {
    .type   = VMEM_TYPE_DRIVER,
    .name   = "siphra",
    .size   = _SIPHRA_VMEM_SIZE,
    .read   = vmem_siphra_read,
    .write  = vmem_siphra_write,
    .driver = NULL,
    .vaddr  = 0,
};

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA0_REGS,
    siphra0,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA0_START,
    "Siphra #0 Registers");

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA1_REGS,
    siphra1,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA1_START,
    "Siphra #1 Registers");

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA2_REGS,
    siphra2,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA2_START,
    "Siphra #2 Registers");

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA3_REGS,
    siphra3,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA3_START,
    "Siphra #3 Registers");

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA4_REGS,
    siphra4,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA4_START,
    "Siphra #4 Registers");

PARAM_DEFINE_STATIC_VMEM(
    PARAMID_SIPHRA5_REGS,
    siphra5,
    PARAM_TYPE_UINT32,
    NUM_SIPHRA_REGISTERS,
    sizeof(uint32_t),
    PM_HWREG,
    NULL,
    NULL,
    siphra,
    _SIPHRA5_START,
    "Siphra #5 Registers");

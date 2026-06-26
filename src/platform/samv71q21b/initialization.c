#include "err_codes.h"
#include "pins.h"

#include <component/pio.h>
#include <ethernet_phy.h>
#include <hpl_gpio.h>
#include <hri_v71b.h>
#include <ieee8023_mii_standard_config.h>
#include <stdint.h>
#include <stdio_io.h>

#include "hal_ext_irq.h"
#include "hal_gpio.h"
#include "hal_init.h"
#include "hal_mac_async.h"
#include "hal_usart_sync.h"

#include "hpl_pmc.h"
#include "hpl_usart_base.h"

struct usart_sync_descriptor TARGET_IO;

struct mac_async_descriptor MACIF;

struct ethernet_phy_descriptor MACIF_PHY_desc;

static int32_t usart_init(void) {
    _pmc_enable_periph_clock(ID_USART1);
    gpio_set_pin_function(PA21, MUX_PA21A_USART1_RXD1);
    gpio_set_pin_function(PB4, MUX_PB4D_USART1_TXD1);
    return usart_sync_init(&TARGET_IO, USART1, _usart_get_usart_sync());
}

static int32_t ethernet_init(void) {

    int32_t status = ERR_NONE;

    // MAC
    _pmc_enable_periph_clock(ID_GMAC);
    gpio_set_pin_function(PD8, MUX_PD8A_GMAC_GMDC);
    gpio_set_pin_function(PD9, MUX_PD9A_GMAC_GMDIO);
    gpio_set_pin_function(PD5, MUX_PD5A_GMAC_GRX0);
    gpio_set_pin_function(PD6, MUX_PD6A_GMAC_GRX1);
    gpio_set_pin_function(PD4, MUX_PD4A_GMAC_GRXDV);
    gpio_set_pin_function(PD7, MUX_PD7A_GMAC_GRXER);
    gpio_set_pin_function(PD2, MUX_PD2A_GMAC_GTX0);
    gpio_set_pin_function(PD3, MUX_PD3A_GMAC_GTX1);
    gpio_set_pin_function(PD0, MUX_PD0A_GMAC_GTXCK);
    gpio_set_pin_function(PD1, MUX_PD1A_GMAC_GTXEN);

    status = mac_async_init(&MACIF, GMAC);
    if (status != ERR_NONE)
        return status;

    status = mac_async_enable(&MACIF);
    if (status != ERR_NONE)
        return status;

    status = ethernet_phy_init(&MACIF_PHY_desc, &MACIF, CONF_MACIF_PHY_IEEE8023_MII_PHY_ADDRESS);
    if (status != ERR_NONE)
        return status;

#if CONF_MACIF_PHY_IEEE8023_MII_CONTROL_REG0_SETTING == 1
    status = ethernet_phy_write_reg(&MACIF_PHY_desc, MDIO_REG0_BMCR, CONF_MACIF_PHY_IEEE8023_MII_CONTROL_REG0);
#endif /* CONF_MACIF_PHY_IEEE8023_MII_CONTROL_REG0_SETTING */

    return status;
}

static void gpio_init(void) {
    _pmc_enable_periph_clock(ID_PIOA);
    gpio_set_pin_level(LED0, false);
    gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SW0, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SW0, GPIO_PULL_UP);
    gpio_set_pin_function(SW0, GPIO_PIN_FUNCTION_OFF);

    // Setup Debounce filter for SW0
    void* base_reg = port_to_reg(GPIO_PORT(SW0));
    // enable filter
    hri_pio_set_IFSR_reg(base_reg, 1 << GPIO_PIN(SW0));
    // select debounce filter
    hri_pio_set_IFSCSR_reg(base_reg, 1 << GPIO_PIN(SW0));
    // debounce filter filter pulses shorter than 1/2 of the programmable divided slow clock.
    // Slow clock is f_slck = 32kHz
    // the divided clock is programmed in the DIV field of PIO_SCDR register:
    // t_div_slck = ((DIV + 1) * 2) * t_slck
    hri_pio_ifscsr_reg_t div = 1024;
    hri_pio_write_SCDR_DIV_bf(base_reg, div);

    _pmc_enable_periph_clock(ID_PIOC);
    gpio_set_pin_level(PHY_RESET_PIN, true);
    gpio_set_pin_direction(PHY_RESET_PIN, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(PHY_RESET_PIN, GPIO_PIN_FUNCTION_OFF);
}

static void nvic_init(void) {

    // turn on priority preemption.
    NVIC_SetPriorityGrouping(0x00);

    __DMB();
    __enable_irq();

    /* Enable Usage fault */
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk);
    /* Trap divide by zero */
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

    /* Enable Bus fault */
    SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk);

    /* Enable memory management fault */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk);

    // Set Interrupt priorities. The interrupts will get enabled in their corresponding init functions.
    NVIC_SetPriority(SysTick_IRQn, 7);
    NVIC_SetPriority(PendSV_IRQn, 7);
    NVIC_SetPriority(SVCall_IRQn, 0);
    NVIC_SetPriority(GMAC_IRQn, 6);
    NVIC_SetPriority(PIOA_IRQn, 6);
}

int32_t board_init(void) {

    int32_t status = ERR_NONE;

    init_mcu();

    /* Disable Watchdog */
    hri_wdt_set_MR_WDDIS_bit(WDT);

    gpio_init();
    ext_irq_init();

    status = ethernet_init();
    if (status != ERR_NONE)
        return status;

    // STDIO
    status = usart_init();
    if (status != ERR_NONE)
        return status;

    status = usart_sync_enable(&TARGET_IO);
    if (status != ERR_NONE)
        return status;

    stdio_io_init(&TARGET_IO.io);

    nvic_init();

    return ERR_NONE;
}

/*******************************************************************************
 End of File
*/

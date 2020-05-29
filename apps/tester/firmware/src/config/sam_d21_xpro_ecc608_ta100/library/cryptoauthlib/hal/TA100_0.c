/*
 * Code generated from MPLAB Harmony.
 *
 * This file will be overwritten when reconfiguring your MPLAB Harmony project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include "cryptoauthlib.h"

ATCAIfaceCfg ta100_0_init_data = {
    .iface_type            = ATCA_SPI_IFACE,
    .devtype               = TA100,
    .atcaspi.bus           = 0,
    .atcaspi.select_pin    = PORT_PIN_PA05,
    .atcaspi.baud          = 1000000,
    .wake_delay            = 1500,
    .rx_retries            = 20,
    .cfg_data              = &sercom0_plib_spi_api
};

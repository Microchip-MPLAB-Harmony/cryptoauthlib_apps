/*
 * Code generated from MPLAB Harmony.
 *
 * This file will be overwritten when reconfiguring your MPLAB Harmony project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include "cryptoauthlib.h"

ATCAIfaceCfg atecc608a_0_init_data = {
       .iface_type            = ATCA_I2C_IFACE,
       .devtype               = ATECC608A,
       .atcai2c.slave_address = 0xC0,
       .atcai2c.bus           = 0,
       .atcai2c.baud          = 400,
       .wake_delay            = 1500,
       .rx_retries            = 20,
       .cfg_data              = &sercom2_plib_api
};
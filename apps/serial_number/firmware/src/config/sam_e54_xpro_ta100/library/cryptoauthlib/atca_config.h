/* Auto-generated config file atca_config.h */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* MPLAB Harmony Common Include */
#include "definitions.h"


#ifndef ATCA_HAL_SPI
#define ATCA_HAL_SPI
#endif

/** Include Device Support Options */
#define ATCA_TA100_SUPPORT


/* Polling Configuration Options  */
#ifndef ATCA_POLLING_INIT_TIME_MSEC
#define ATCA_POLLING_INIT_TIME_MSEC       1
#endif
#ifndef ATCA_POLLING_FREQUENCY_TIME_MSEC
#define ATCA_POLLING_FREQUENCY_TIME_MSEC  2
#endif
#ifndef ATCA_POLLING_MAX_TIME_MSEC
#define ATCA_POLLING_MAX_TIME_MSEC        2500
#endif

/** Define if the library is not to use malloc/free */
#ifndef ATCA_NO_HEAP
#define ATCA_NO_HEAP
#endif

#define atca_delay_ms   hal_delay_ms
#define atca_delay_us   hal_delay_us

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif


/* Define generic interfaces to the processor libraries */

typedef bool (* atca_spi_plib_read)( void * , size_t );
typedef bool (* atca_spi_plib_write)( void *, size_t );
typedef bool (* atca_spi_plib_is_busy)( void );
typedef void (* atca_spi_plib_select)(uint32_t pin, bool value);

typedef struct atca_plib_spi_api
{
    atca_spi_plib_read              read;
    atca_spi_plib_write             write;
    atca_spi_plib_is_busy           is_busy;
    atca_spi_plib_select            select;
} atca_plib_spi_api_t;

extern atca_plib_spi_api_t sercom4_plib_spi_api;




#endif // ATCA_CONFIG_H

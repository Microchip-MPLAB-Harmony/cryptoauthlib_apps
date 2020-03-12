/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "cryptoauthlib.h"

#if defined ATCA_ATECC608A_SUPPORT
extern ATCAIfaceCfg atecc608a_0_init_data;
ATCAIfaceCfg* device_cfg = &atecc608a_0_init_data;
#elif defined ATCA_ATECC508A_SUPPORT
extern ATCAIfaceCfg atecc508a_0_init_data;
ATCAIfaceCfg* device_cfg = &atecc508a_0_init_data;
#endif

uint8_t sernum[9];
char displayStr[ATCA_SERIAL_NUM_SIZE * 3];
size_t displen = sizeof(displayStr);
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    ATCA_STATUS status;

    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    printf("Starting Cryptoauth Serial Number Example\r\n");
    
    status = atcab_init(device_cfg);
    printf("atcab_init: %d\r\n", status);
    if (ATCA_SUCCESS == status)
    {
        status = atcab_read_serial_number(sernum);
        atcab_bin2hex(sernum, 9, displayStr, &displen);
        printf("Serial Number of the Device: %s\r\n\n", displayStr);
    }
    
    atcab_release();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/


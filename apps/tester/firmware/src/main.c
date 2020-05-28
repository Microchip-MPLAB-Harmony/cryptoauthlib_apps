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

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

/* Cryptoauth Device Library Header */
#include "cryptoauthlib.h"

/* Tester Application for Cryptoauthlib */
#include "atca_test.h"

#ifdef ATCA_ATECC508A_SUPPORT
extern ATCAIfaceCfg atecc508a_0_init_data;
#endif

#ifdef ATCA_ATECC608A_SUPPORT
extern ATCAIfaceCfg atecc608a_0_init_data;
#endif

#ifdef ATCA_TA100_SUPPORT
extern ATCAIfaceCfg ta100_0_init_data;
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    printf("Starting Cryptoauth Tester Example: Type help <Return> for menu\r\n");
    
#ifdef ATCA_ATECC508A_SUPPORT
    atca_test_config_set_ifacecfg(&atecc508a_0_init_data);
#elif defined(ATCA_ATECC608A_SUPPORT)
    atca_test_config_set_ifacecfg(&atecc608a_0_init_data);
#elif defined(ATCA_TA100_SUPPORT)
    atca_test_config_set_ifacecfg(&ta100_0_init_data);
#endif
    atca_test_task();
    
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


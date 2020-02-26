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
#include "cryptoauthlib.h"
#include "atcacert/atcacert_pem.h"
#include "atcacert/atcacert_host_hw.h"
#include "tng/tng_atca.h"
#include "tng/tng_atcacert_client.h"

extern ATCAIfaceCfg atecc608a_0_init_data;

// *****************************************************************************
// *****************************************************************************
// Section : Global Variable
// *****************************************************************************
// *****************************************************************************
size_t g_signer_cert_size;
size_t g_device_cert_size;
uint8_t g_signer_cert[1024];
uint8_t g_device_cert[1024];


ATCA_STATUS print_cert_pem_format(uint8_t* g_cert, size_t g_cert_size)
{
    ATCA_STATUS status;
    char g_cert_pem[1024];
    size_t g_cert_size_pem = 1024;
    status = atcacert_encode_pem_cert(g_cert, g_cert_size, g_cert_pem, &g_cert_size_pem);
    if (status != ATCACERT_E_SUCCESS)
    {
        printf("Encoding PEM certificate failed with an error %d\r\n",status);
        return status;
    }
    printf("%s\r\n",g_cert_pem);
    return status;
}

ATCA_STATUS tng_get_signer_cert_def(const atcacert_def_t** cert_def)
{
    ATCA_STATUS status = tng_get_device_cert_def(cert_def);

    if(ATCA_SUCCESS == status)
    {
        *cert_def = (*cert_def)->ca_cert_def;
    }

    return status;
}

ATCA_STATUS verify_chain_cert()
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t public_key[64];
    const atcacert_def_t* cert_def = NULL;

    tng_atcacert_root_public_key(public_key);

    if((status = tng_get_signer_cert_def(&cert_def)) != ATCA_SUCCESS)
    {
        return status;
    }

    if((status = atcacert_verify_cert_hw(cert_def, g_signer_cert, g_signer_cert_size, public_key)) != ATCACERT_E_SUCCESS)
    {
        printf("Signer certificate is not verified against Root CA, failed with an error %d\r\n",status);
        return status;
    }
    printf("Signer Certificate is validated against Root CA certificate\r\n");

    tng_atcacert_signer_public_key(public_key, g_signer_cert);

    if((status = tng_get_device_cert_def(&cert_def)) != ATCA_SUCCESS)
    {
        return status;
    }

    if((status = atcacert_verify_cert_hw(cert_def, g_device_cert, g_device_cert_size, public_key)) != ATCACERT_E_SUCCESS)
    {
        printf("Device Certificate is not verified against Signer Certificate, failed with an error %d\r\n",status);
        return status;
    }
    printf("Device Certificate is validated against Signer Certificate\r\n");

    return status;
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    ATCA_STATUS status;
    uint8_t g_root_cert[1024];
    size_t g_root_cert_size;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    printf("Starting TNG certificate Example\r\n\n");

    status = atcab_init(&atecc608a_0_init_data);

    do
    {
        tng_atcacert_root_cert_size(&g_root_cert_size);
        if((status = tng_atcacert_root_cert(g_root_cert, &g_root_cert_size)) != ATCACERT_E_SUCCESS)
        {
            printf("Root certificate loading failed with an error %d\r\n",status);
            break;
        }
        printf("Root Certificate loading from device\r\n");
        if(print_cert_pem_format(g_root_cert, g_root_cert_size) != ATCACERT_E_SUCCESS)
        {
            break;
        }

        tng_atcacert_max_signer_cert_size(&g_signer_cert_size);
        if((status = tng_atcacert_read_signer_cert(g_signer_cert, &g_signer_cert_size)) != ATCACERT_E_SUCCESS)
        {
            printf("Signer Certificate loading failed with an error %d\r\n",status);
            break;
        }
        printf("Signer Certificate loading from device\r\n");
        if(print_cert_pem_format(g_signer_cert, g_signer_cert_size) != ATCACERT_E_SUCCESS)
        {
            break;
        }

        tng_atcacert_max_device_cert_size(&g_device_cert_size);
        if((status = tng_atcacert_read_device_cert(g_device_cert, &g_device_cert_size, g_signer_cert)) != ATCACERT_E_SUCCESS)
        {
            printf("Device Certificate loading failed with an error %d\r\n", status);
        }
        printf("Device Certificate loading from device\r\n");
        if(print_cert_pem_format(g_device_cert, g_device_cert_size) != ATCACERT_E_SUCCESS)
        {
            break;
        }

        printf("Verifying Chain of Trust:\r\n\n");
        verify_chain_cert();

    }while(0);

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


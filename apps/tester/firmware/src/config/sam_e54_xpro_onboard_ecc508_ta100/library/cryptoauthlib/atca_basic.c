/**
 * \file
 * \brief CryptoAuthLib Basic API methods. These methods provide a simpler way
 *        to access the core crypto methods.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"
#include "atca_basic.h"
#include "atca_version.h"

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
#if defined(_MSC_VER)
#pragma message("Warning : Using a constant host nonce with atcab_read_enc, atcab_write_enc, etcc., can allow spoofing of a device by replaying previously recorded messages")
#else
#warning "Using a constant host nonce with atcab_read_enc, atcab_write_enc, etcc., can allow spoofing of a device by replaying previously recorded messages"
#endif
#endif

const char atca_version[] = ATCA_LIBRARY_VERSION_DATE;
SHARED_LIB_EXPORT ATCADevice _gDevice = NULL;
#ifdef ATCA_NO_HEAP
SHARED_LIB_EXPORT struct atca_command g_atcab_command;
SHARED_LIB_EXPORT struct atca_iface g_atcab_iface;
SHARED_LIB_EXPORT struct atca_device g_atcab_device;
#endif

/** \brief basic API methods are all prefixed with atcab_  (CryptoAuthLib Basic)
 *  the fundamental premise of the basic API is it is based on a single interface
 *  instance and that instance is global, so all basic API commands assume that
 *  one global device is the one to operate on.
 */

/** \brief returns a version string for the CryptoAuthLib release.
 *  The format of the version string returned is "yyyymmdd"
 * \param[out] ver_str ptr to space to receive version string
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_version(char *ver_str)
{
    strcpy(ver_str, atca_version);
    return ATCA_SUCCESS;
}


/** \brief Creates a global ATCADevice object used by Basic API.
 *  \param[in] cfg  Logical interface configuration. Some predefined
 *                  configurations can be found in atca_cfgs.h
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_init(ATCAIfaceCfg *cfg)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // If a device has already been initialized, release it
    if (_gDevice)
    {
        atcab_release();
    }

#ifdef ATCA_NO_HEAP
    g_atcab_device.mCommands = &g_atcab_command;
    g_atcab_device.mIface = &g_atcab_iface;
    status = initATCADevice(cfg, &g_atcab_device);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    _gDevice = &g_atcab_device;
#else
    _gDevice = newATCADevice(cfg);
    if (_gDevice == NULL)
    {
        return ATCA_GEN_FAIL;
    }
#endif

#ifdef ATCA_ATECC608A_SUPPORT
    if (cfg->devtype == ATECC608A)
    {
        if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, ATCA_CHIPMODE_OFFSET, &_gDevice->mCommands->clock_divider, 1)) != ATCA_SUCCESS)
        {
            return status;
        }
        _gDevice->mCommands->clock_divider &= ATCA_CHIPMODE_CLOCK_DIV_MASK;
    }
#endif

    return ATCA_SUCCESS;
}

/** \brief Initialize the global ATCADevice object to point to one of your
 *         choosing for use with all the atcab_ basic API.
 *  \param[in] ca_device  ATCADevice instance to use as the global Basic API
 *                        crypto device instance
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_init_device(ATCADevice ca_device)
{
    if (ca_device == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if (atGetCommands(ca_device) == NULL || atGetIFace(ca_device) == NULL)
    {
        return ATCA_GEN_FAIL;
    }

    // if there's already a device created, release it
    if (_gDevice)
    {
        atcab_release();
    }

    _gDevice = ca_device;

    return ATCA_SUCCESS;
}

/** \brief release (free) the global ATCADevice instance.
 *  This must be called in order to release or free up the interface.
 *  \return Returns ATCA_SUCCESS .
 */
ATCA_STATUS atcab_release(void)
{
#ifdef ATCA_NO_HEAP
    ATCA_STATUS status = releaseATCADevice(_gDevice);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    _gDevice = NULL;
#else
    deleteATCADevice(&_gDevice);
#endif
    return ATCA_SUCCESS;
}

/** \brief Get the global device object.
 *  \return instance of global ATCADevice
 */
ATCADevice atcab_get_device(void)
{
    return _gDevice;
}

/** \brief Get the current device type.
 *  \return Device type if basic api is initialized or ATCA_DEV_UNKNOWN.
 */
ATCADeviceType atcab_get_device_type(void)
{
    ATCADeviceType ret = ATCA_DEV_UNKNOWN;
    if (_gDevice && _gDevice->mIface && _gDevice->mIface->mIfaceCFG)
    {
        ret = _gDevice->mIface->mIfaceCFG->devtype;
    }
    return ret;
}

bool atcab_is_ca_device(ATCADeviceType dev_type)
{
    return (dev_type < TA100) ? true : false;
}

bool atcab_is_ta_device(ATCADeviceType dev_type)
{
    return (dev_type == TA100) ? true : false;
}

#if ATCA_TA_SUPPORT && ATCA_CA_SUPPORT

ATCA_STATUS atcab_wakeup(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_wakeup(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_SUCCESS;
    }
    return status;
}

ATCA_STATUS atcab_idle(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_idle(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_SUCCESS;
    }
    return status;
}

ATCA_STATUS atcab_sleep(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sleep(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_SUCCESS;
    }
    return status;
}

ATCA_STATUS atcab_get_zone_size(uint8_t zone, uint16_t slot, size_t* size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_get_zone_size(_gDevice, zone, slot,  size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_get_zone_size(_gDevice, zone, slot, size);
    }
    return status;
}

/* AES commands */
ATCA_STATUS atcab_aes(uint8_t mode, uint16_t key_id, const uint8_t* aes_in, uint8_t* aes_out)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608A_SUPPORT)
        status = calib_aes(_gDevice, mode, key_id, aes_in, aes_out);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_aes_encrypt(uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608A_SUPPORT)
        status = calib_aes_encrypt(_gDevice, key_id, key_block, plaintext, ciphertext);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_aes_encrypt(_gDevice, key_id, key_block, plaintext, ciphertext);
    }
    return status;
}

ATCA_STATUS atcab_aes_decrypt(uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608A_SUPPORT)
        status = calib_aes_decrypt(_gDevice, key_id, key_block, ciphertext, plaintext);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_aes_decrypt(_gDevice, key_id, key_block, ciphertext, plaintext);
    }
    return status;
}

ATCA_STATUS atcab_aes_gfm(const uint8_t* h, const uint8_t* input, uint8_t* output)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATECC608A_SUPPORT)
        status = calib_aes_gfm(_gDevice, h, input, output);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/*
ATCA_STATUS atcab_aes_gcm_init(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv, size_t iv_size);
ATCA_STATUS atcab_aes_gcm_init_rand(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, size_t rand_size,
    const uint8_t* free_field, size_t free_field_size, uint8_t* iv);
ATCA_STATUS atcab_aes_gcm_aad_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* aad, uint32_t aad_size);
ATCA_STATUS atcab_aes_gcm_encrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_gcm_encrypt_finish(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size);
ATCA_STATUS atcab_aes_gcm_decrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext);
ATCA_STATUS atcab_aes_gcm_decrypt_finish(atca_aes_gcm_ctx_t* ctx, const uint8_t* tag, size_t tag_size, bool* is_verified);
*/

/* CheckMAC command */
ATCA_STATUS atcab_checkmac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, const uint8_t* response, const uint8_t* other_data)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_checkmac(_gDevice, mode, key_id, challenge, response, other_data);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* Counter command */
ATCA_STATUS atcab_counter(uint8_t mode, uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_counter(_gDevice, mode, counter_id, counter_value);
    }
    else if (atcab_is_ta_device(dev_type))
    {
//        status = talib_counter(_gDevice, mode, counter_id, counter_value);
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_counter_increment(uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_counter_increment(_gDevice, counter_id, counter_value);
    }
    else if (atcab_is_ta_device(dev_type))
    {
//        status = talib_counter_increment(_gDevice, counter_id, counter_value);
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_counter_read(uint16_t counter_id, uint32_t* counter_value)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_counter_read(_gDevice, counter_id, counter_value);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_counter_read(_gDevice, counter_id, counter_value);
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* DeriveKey command */
ATCA_STATUS atcab_derivekey(uint8_t mode, uint16_t key_id, const uint8_t* mac)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_derivekey(_gDevice, mode, key_id, mac);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* ECDH command */
ATCA_STATUS atcab_ecdh_base(uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_ecdh_base(_gDevice, mode, key_id, public_key, pms, out_nonce);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_ecdh(uint16_t key_id, const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_ecdh(_gDevice, key_id, public_key, pms);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_ecdh_compat(_gDevice, key_id, public_key, pms);
    }
    return status;
}

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id)
#else
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
#ifdef ATCA_USE_CONSTANT_HOST_NONCE
        status = calib_ecdh_enc(_gDevice, key_id, public_key, pms, read_key, read_key_id);
#else
        status = calib_ecdh_enc(_gDevice, key_id, public_key, pms, read_key, read_key_id, num_in);
#endif
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_ecdh_ioenc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608A_SUPPORT
        status = calib_ecdh_ioenc(_gDevice, key_id, public_key, pms, io_key);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_ecdh_tempkey(const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608A_SUPPORT
        status = calib_ecdh_tempkey(_gDevice, public_key, pms);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_ecdh_tempkey_ioenc(const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ATECC608A_SUPPORT
        status = calib_ecdh_tempkey_ioenc(_gDevice, public_key, pms, io_key);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* GenDig command */
ATCA_STATUS atcab_gendig(uint8_t zone, uint16_t key_id, const uint8_t* other_data, uint8_t other_data_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_gendig(_gDevice, zone, key_id, other_data, other_data_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* GenKey command */
ATCA_STATUS atcab_genkey_base(uint8_t mode, uint16_t key_id, const uint8_t* other_data, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_genkey_base(_gDevice, mode, key_id, other_data, public_key);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_genkey(uint16_t key_id, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_genkey(_gDevice, key_id, public_key);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_genkey_compat(_gDevice, key_id, public_key);
    }
    return status;
}

ATCA_STATUS atcab_get_pubkey(uint16_t key_id, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_get_pubkey(_gDevice, key_id, public_key);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_get_pubkey_compat(_gDevice, key_id, public_key);
    }
    return status;
}

// HMAC command functions
ATCA_STATUS atcab_hmac(uint8_t mode, uint16_t key_id, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_ATSHA204A_SUPPORT) || defined(ATCA_ATECC508A_SUPPORT)
        status = calib_hmac(_gDevice, mode, key_id, digest);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// Info command functions
ATCA_STATUS atcab_info_base(uint8_t mode, uint16_t param2, uint8_t* out_data)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_info_base(_gDevice, mode, param2, out_data);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_info(uint8_t* revision)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_info(_gDevice, revision);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_info_compat(_gDevice, revision);
    }
    return status;
}

ATCA_STATUS atcab_info_set_latch(bool state)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_info_set_latch(_gDevice, state);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_info_get_latch(bool* state)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_info_get_latch(_gDevice, state);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// KDF command functions
ATCA_STATUS atcab_kdf(uint8_t mode, uint16_t key_id, const uint32_t details, const uint8_t* message, uint8_t* out_data, uint8_t* out_nonce)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_kdf(_gDevice, mode, key_id, details, message, out_data, out_nonce);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* Lock commands */

ATCA_STATUS atcab_lock(uint8_t mode, uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock(_gDevice, mode, summary_crc);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_lock_config_zone(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock_config_zone(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_lock_config(_gDevice);
    }
    return status;
}

ATCA_STATUS atcab_lock_config_zone_crc(uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock_config_zone_crc(_gDevice, summary_crc);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_lock_config_with_crc(_gDevice, summary_crc);
    }
    return status;
}

ATCA_STATUS atcab_lock_data_zone(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock_data_zone(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_lock_setup(_gDevice);
    }
    return status;
}

ATCA_STATUS atcab_lock_data_zone_crc(uint16_t summary_crc)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock_data_zone_crc(_gDevice, summary_crc);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_lock_data_slot(uint16_t slot)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_lock_data_slot(_gDevice, slot);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_lock_handle(_gDevice, slot);
    }
    return status;
}

// MAC command functions
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_mac(_gDevice, mode, key_id, challenge, digest);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// Nonce command functions
ATCA_STATUS atcab_nonce_base(uint8_t mode, uint16_t zero, const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_nonce_base(_gDevice, mode, zero, num_in, rand_out);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_nonce(const uint8_t* num_in)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_nonce(_gDevice, num_in);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_nonce_load(uint8_t target, const uint8_t* num_in, uint16_t num_in_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_nonce_load(_gDevice, target, num_in, num_in_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_nonce_rand(const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_nonce_rand(_gDevice, num_in, rand_out);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_challenge(const uint8_t* num_in)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_challenge(_gDevice, num_in);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_challenge_seed_update(const uint8_t* num_in, uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_challenge_seed_update(_gDevice, num_in, rand_out);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// PrivWrite command functions
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32])
#else
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32], const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
#ifdef ATCA_USE_CONSTANT_HOST_NONCE
        status = calib_priv_write(_gDevice, key_id, priv_key, write_key_id, write_key);
#else
        status = calib_priv_write(_gDevice, key_id, priv_key, write_key_id, write_key, num_in);
#endif /* ATCA_USE_CONSTANT_HOST_NONCE */
#else
        status = ATCA_UNIMPLEMENTED;
#endif /* ATCA_ECC_SUPPORT */
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// Random command functions
ATCA_STATUS atcab_random(uint8_t* rand_out)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_random(_gDevice, rand_out);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_random_compat(_gDevice, rand_out);
    }
    return status;
}

// Read command functions
ATCA_STATUS atcab_read_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t* data, uint8_t len)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_zone(_gDevice, zone, slot, block, offset, data, len);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_is_locked(uint8_t zone, bool* is_locked)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_is_locked(_gDevice, zone, is_locked);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        if (LOCK_ZONE_CONFIG == zone)
        {
            status = talib_is_config_locked(_gDevice, is_locked);
        }
        else if (LOCK_ZONE_DATA == zone)
        {
            status = talib_is_setup_locked(_gDevice, is_locked);
        }
        else
        {
            status = TRACE(ATCA_BAD_PARAM, "");
        }
    }
    return status;
}

ATCA_STATUS atcab_is_config_locked(bool* is_locked)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_is_locked(_gDevice, LOCK_ZONE_CONFIG, is_locked);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_is_config_locked(_gDevice, is_locked);
    }
    return status;
}

ATCA_STATUS atcab_is_data_locked( bool* is_locked)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_is_locked(_gDevice, LOCK_ZONE_DATA, is_locked);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_is_setup_locked(_gDevice, is_locked);
    }
    return status;
}

ATCA_STATUS atcab_is_slot_locked(uint16_t slot, bool* is_locked)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_is_slot_locked(_gDevice, slot, is_locked);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_is_handle_locked(_gDevice, slot, is_locked);
    }
    return status;
}

ATCA_STATUS atcab_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset, uint8_t* data, size_t length)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_bytes_zone(_gDevice, zone, slot, offset, data, length);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_read_bytes_zone(_gDevice, zone, slot, offset, data, length);
    }
    return status;
}

ATCA_STATUS atcab_read_serial_number(uint8_t* serial_number)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_serial_number(_gDevice, serial_number);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_info_serial_number_compat(_gDevice, serial_number);
    }
    return status;
}

ATCA_STATUS atcab_read_pubkey(uint16_t slot, uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_pubkey(_gDevice, slot, public_key);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_read_pubkey_compat(_gDevice, slot, public_key);
    }
    return status;
}

ATCA_STATUS atcab_read_sig(uint16_t slot, uint8_t* sig)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_sig(_gDevice, slot, sig);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_read_sig_compat(_gDevice, slot, sig);
    }
    return status;
}

ATCA_STATUS atcab_read_config_zone(uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_read_config_zone(_gDevice, config_data);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_read_config_zone(_gDevice, config_data);
    }
    return status;
}

ATCA_STATUS atcab_cmp_config_zone(uint8_t* config_data, bool* same_config)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_cmp_config_zone(_gDevice, config_data, same_config);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_cmp_config_zone(_gDevice, config_data, same_config);
    }
    return status;
}

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id)
#else
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        status = calib_read_enc(_gDevice, key_id, block, data, enc_key, enc_key_id);
#else
        status = calib_read_enc(_gDevice, key_id, block, data, enc_key, enc_key_id, num_in);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

// SecureBoot command functions
ATCA_STATUS atcab_secureboot(uint8_t mode, uint16_t param2, const uint8_t* digest, const uint8_t* signature, uint8_t* mac)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_secureboot(_gDevice, mode, param2, digest, signature, mac);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_secureboot_mac(uint8_t mode, const uint8_t* digest, const uint8_t* signature, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_secureboot_mac(_gDevice, mode, digest, signature, num_in, io_key, is_verified);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* SelfTest Command */
ATCA_STATUS atcab_selftest(uint8_t mode, uint16_t param2, uint8_t* result)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_selftest(_gDevice, mode, param2, result);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* SHA Command  */
ATCA_STATUS atcab_sha_base(uint8_t mode, uint16_t length, const uint8_t* data_in, uint8_t* data_out, uint16_t* data_out_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_base(_gDevice, mode, length, data_in, data_out, data_out_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_base_compat(_gDevice, mode, length, data_in, data_out, data_out_size);
    }
    return status;
}

ATCA_STATUS atcab_sha_start(void)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_start(_gDevice);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_start(_gDevice);
    }
    return status;
}

ATCA_STATUS atcab_sha_update(const uint8_t* message)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_update(_gDevice, message);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_update_compat(_gDevice, message);
    }
    return status;
}

ATCA_STATUS atcab_sha_end(uint8_t* digest, uint16_t length, const uint8_t* message)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_end(_gDevice, digest, length, message);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_end_compat(_gDevice, digest, length, message);
    }
    return status;
}

ATCA_STATUS atcab_sha_read_context(uint8_t* context, uint16_t* context_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_read_context(_gDevice, context, context_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_read_context(_gDevice, context, context_size);
    }
    return status;
}

ATCA_STATUS atcab_sha_write_context(const uint8_t* context, uint16_t context_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_write_context(_gDevice, context, context_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha_write_context(_gDevice, context, context_size);
    }
    return status;
}

ATCA_STATUS atcab_sha(uint16_t length, const uint8_t* message, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha(_gDevice, length, message, digest);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha(_gDevice, length, message, digest);
    }
    return status;
}

ATCA_STATUS atcab_hw_sha2_256(const uint8_t* data, size_t data_size, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_hw_sha2_256(_gDevice, data, data_size, digest);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sha(_gDevice, (uint16_t)data_size, data, digest);
    }
    return status;
}

ATCA_STATUS atcab_hw_sha2_256_init(atca_sha256_ctx_t* ctx)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_hw_sha2_256_init(_gDevice, ctx);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_hw_sha2_256_update(atca_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_hw_sha2_256_update(_gDevice, ctx, data, data_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_hw_sha2_256_finish(atca_sha256_ctx_t* ctx, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_hw_sha2_256_finish(_gDevice, ctx, digest);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_sha_hmac_init(atca_hmac_sha256_ctx_t* ctx, uint16_t key_slot)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_hmac_init(_gDevice, ctx, key_slot);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_sha_hmac_update(atca_hmac_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_hmac_update(_gDevice, ctx, data, data_size);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_sha_hmac_finish(atca_hmac_sha256_ctx_t* ctx, uint8_t* digest, uint8_t target)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_hmac_finish(_gDevice, ctx, digest, target);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_sha_hmac(const uint8_t* data, size_t data_size, uint16_t key_slot, uint8_t* digest, uint8_t target)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_sha_hmac(_gDevice, data, data_size, key_slot, digest, target);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_hmac_compat(_gDevice, data, data_size, key_slot, digest, target);
    }
    return status;
}


/* Sign command */
ATCA_STATUS atcab_sign_base(uint8_t mode, uint16_t key_id, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_sign_base(_gDevice, mode, key_id, signature);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_sign(uint16_t key_id, const uint8_t* msg, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_sign(_gDevice, key_id, msg, signature);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_sign_compat(_gDevice, key_id, msg, signature);
    }
    return status;
}

ATCA_STATUS atcab_sign_internal(uint16_t key_id, bool is_invalidate, bool is_full_sn, uint8_t* signature)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_sign_internal(_gDevice, key_id, is_invalidate, is_full_sn, signature);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* UpdateExtra command */
ATCA_STATUS atcab_updateextra(uint8_t mode, uint16_t new_value)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_updateextra(_gDevice, mode, new_value);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* Verify command */
ATCA_STATUS atcab_verify(uint8_t mode, uint16_t key_id, const uint8_t* signature, const uint8_t* public_key, const uint8_t* other_data, uint8_t* mac)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify(_gDevice, mode, key_id, signature, public_key, other_data, mac);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_verify_extern(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_extern(_gDevice, message, signature, public_key, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_verify_extern_compat(_gDevice, message, signature, public_key, is_verified);
    }
    return status;
}

ATCA_STATUS atcab_verify_extern_mac(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_extern_mac(_gDevice, message, signature, public_key, num_in, io_key, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_verify_stored(const uint8_t* message, const uint8_t* signature, uint16_t key_id, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_stored(_gDevice, message, signature, key_id, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_verify_stored_compat(_gDevice, message, signature, key_id, is_verified);
    }
    return status;
}

ATCA_STATUS atcab_verify_stored_mac(const uint8_t* message, const uint8_t* signature, uint16_t key_id, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_stored_mac(_gDevice, message, signature, key_id, num_in, io_key, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_verify_validate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_validate(_gDevice, key_id, signature, other_data, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_verify_invalidate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#ifdef ATCA_ECC_SUPPORT
        status = calib_verify_invalidate(_gDevice, key_id, signature, other_data, is_verified);
#else
        status = ATCA_UNIMPLEMENTED;
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

/* Write command functions */
ATCA_STATUS atcab_write(uint8_t zone, uint16_t address, const uint8_t* value, const uint8_t* mac)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write(_gDevice, zone, address, value, mac);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}
ATCA_STATUS atcab_write_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t* data, uint8_t len)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write_zone(_gDevice, zone, slot, block, offset, data, len);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_write_zone(_gDevice, zone, slot, block, offset, data, len);
    }
    return status;
}

ATCA_STATUS atcab_write_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t* data, size_t length)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write_bytes_zone(_gDevice, zone, slot, offset_bytes, data, length);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_write_bytes_zone(_gDevice, zone, slot, offset_bytes, data, length);
    }
    return status;
}

ATCA_STATUS atcab_write_pubkey(uint16_t slot, const uint8_t* public_key)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write_pubkey(_gDevice, slot, public_key);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_write_pubkey_compat(_gDevice, slot, public_key);
    }
    return status;
}

ATCA_STATUS atcab_write_config_zone(const uint8_t* config_data)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write_config_zone(_gDevice, config_data);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = talib_write_config_zone(_gDevice, config_data);
    }
    return status;
}

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id)
#else
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        status = calib_write_enc(_gDevice, key_id, block, data, enc_key, enc_key_id);
#else
        status = calib_write_enc(_gDevice, key_id, block, data, enc_key, enc_key_id, num_in);
#endif
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

ATCA_STATUS atcab_write_config_counter(uint16_t counter_id, uint32_t counter_value)
{
    ATCA_STATUS status = ATCA_NOT_INITIALIZED;
    ATCADeviceType dev_type = atcab_get_device_type();

    if (atcab_is_ca_device(dev_type))
    {
        status = calib_write_config_counter(_gDevice, counter_id, counter_value);
    }
    else if (atcab_is_ta_device(dev_type))
    {
        status = ATCA_UNIMPLEMENTED;
    }
    return status;
}

#endif

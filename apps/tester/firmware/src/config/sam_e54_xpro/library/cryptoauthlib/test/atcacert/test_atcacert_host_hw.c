/**
 * \file
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
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

#include "atcacert/atcacert_host_hw.h"
#include "third_party/unity/unity.h"
#include "third_party/unity/unity_fixture.h"
#include "atca_basic.h"
#include "test_cert_def_0_device.h"
#include "test_cert_def_1_signer.h"
#include <string.h>

extern ATCAIfaceCfg *gCfg;

static const uint8_t g_signer_cert[] = {
    0x30, 0x82, 0x01, 0xB0, 0x30, 0x82, 0x01, 0x57, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x03, 0x40,
    0xC4, 0x8B, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x36,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x07, 0x45, 0x78, 0x61, 0x6D, 0x70,
    0x6C, 0x65, 0x31, 0x22, 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x19, 0x45, 0x78, 0x61,
    0x6D, 0x70, 0x6C, 0x65, 0x20, 0x41, 0x54, 0x45, 0x43, 0x43, 0x35, 0x30, 0x38, 0x41, 0x20, 0x52,
    0x6F, 0x6F, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x34, 0x30, 0x38, 0x30, 0x32,
    0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x17, 0x0D, 0x33, 0x34, 0x30, 0x38, 0x30, 0x32, 0x32,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x30, 0x3A, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04,
    0x0A, 0x0C, 0x07, 0x45, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03,
    0x55, 0x04, 0x03, 0x0C, 0x1D, 0x45, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x41, 0x54, 0x45,
    0x43, 0x43, 0x35, 0x30, 0x38, 0x41, 0x20, 0x53, 0x69, 0x67, 0x6E, 0x65, 0x72, 0x20, 0x43, 0x34,
    0x38, 0x42, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06,
    0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x8B, 0x59, 0x97,
    0xE6, 0x3A, 0xD2, 0x18, 0xBF, 0xE6, 0x19, 0xD7, 0x42, 0x17, 0xD8, 0xA7, 0x79, 0x7E, 0xD4, 0x41,
    0xA8, 0x9C, 0x5E, 0x7E, 0x13, 0xAD, 0x7D, 0xA1, 0xBF, 0xA7, 0x71, 0x31, 0x6F, 0xD4, 0xFE, 0x6A,
    0x6A, 0xCD, 0x1D, 0x94, 0x3A, 0x07, 0xCD, 0x3D, 0x7D, 0xD2, 0x0C, 0xCF, 0xF6, 0xCA, 0x04, 0xFC,
    0xBC, 0x15, 0xE8, 0x6C, 0x26, 0x39, 0xE0, 0x1F, 0xAA, 0x6C, 0xA0, 0x4A, 0x12, 0xA3, 0x50, 0x30,
    0x4E, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xFF, 0x30,
    0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xD9, 0x69, 0xCE, 0x9C, 0xE4, 0x33,
    0xEF, 0x2A, 0xEF, 0xCC, 0xD8, 0x62, 0x72, 0x4A, 0x49, 0xA2, 0x1B, 0x17, 0xE5, 0xD3, 0x30, 0x1F,
    0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0xFB, 0x17, 0xB4, 0x6A, 0x07,
    0xCC, 0xCE, 0x70, 0xF5, 0xF7, 0xC2, 0xD7, 0x8D, 0xDD, 0x62, 0x1A, 0x12, 0xF0, 0x9C, 0xD3, 0x30,
    0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x47, 0x00, 0x30, 0x44,
    0x02, 0x20, 0x29, 0x09, 0xF2, 0xE2, 0xE5, 0xB1, 0xF0, 0xF1, 0xE5, 0x37, 0x90, 0x9A, 0x4D, 0x0D,
    0xCB, 0x38, 0x23, 0x0E, 0xE8, 0x5D, 0xC8, 0xF8, 0xAC, 0x07, 0x65, 0x22, 0x9E, 0x11, 0xC3, 0x95,
    0xD7, 0x96, 0x02, 0x20, 0x13, 0xBC, 0x35, 0x0A, 0x93, 0x95, 0xE8, 0xFF, 0x08, 0xFD, 0xEC, 0x51,
    0x11, 0x8D, 0xD0, 0x5F, 0xBD, 0x95, 0xE9, 0x81, 0x92, 0xB3, 0x76, 0x5D, 0xC2, 0xD4, 0xF4, 0x40,
    0xEB, 0x81, 0xE7, 0xF5
};

static const uint8_t g_device_cert[] = {
    0x30, 0x82, 0x01, 0x8A, 0x30, 0x82, 0x01, 0x30, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x0A, 0x40,
    0x01, 0x23, 0x83, 0x32, 0xD9, 0x2C, 0xA5, 0x71, 0xEE, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48,
    0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x3A, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A,
    0x0C, 0x07, 0x45, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03, 0x55,
    0x04, 0x03, 0x0C, 0x1D, 0x45, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x41, 0x54, 0x45, 0x43,
    0x43, 0x35, 0x30, 0x38, 0x41, 0x20, 0x53, 0x69, 0x67, 0x6E, 0x65, 0x72, 0x20, 0x43, 0x34, 0x38,
    0x42, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x35, 0x30, 0x39, 0x30, 0x33, 0x32, 0x31, 0x30, 0x30, 0x30,
    0x30, 0x5A, 0x17, 0x0D, 0x33, 0x35, 0x30, 0x39, 0x30, 0x33, 0x32, 0x31, 0x30, 0x30, 0x30, 0x30,
    0x5A, 0x30, 0x35, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x07, 0x45, 0x78,
    0x61, 0x6D, 0x70, 0x6C, 0x65, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x18,
    0x45, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x41, 0x54, 0x45, 0x43, 0x43, 0x35, 0x30, 0x38,
    0x41, 0x20, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86,
    0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04, 0xC7, 0x94, 0x94, 0x6A, 0x0C, 0xCB, 0x54, 0x1E, 0xFE, 0x50, 0x56, 0xA2, 0x93,
    0xC8, 0xC4, 0xC0, 0xC2, 0x41, 0xC1, 0x35, 0x78, 0xDE, 0x98, 0x19, 0x2C, 0x86, 0x11, 0x5F, 0x4E,
    0x98, 0x10, 0x38, 0xF1, 0x93, 0xCB, 0xA4, 0x81, 0x6A, 0xD8, 0x67, 0x04, 0x4E, 0x98, 0x36, 0x95,
    0x7F, 0xD6, 0xF0, 0x03, 0xA9, 0x82, 0x05, 0x3A, 0xD5, 0x5D, 0x99, 0x2A, 0xD0, 0x00, 0x2F, 0x3D,
    0xFB, 0x8B, 0xCE, 0xA3, 0x23, 0x30, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0xD9, 0x69, 0xCE, 0x9C, 0xE4, 0x33, 0xEF, 0x2A, 0xEF, 0xCC, 0xD8, 0x62,
    0x72, 0x4A, 0x49, 0xA2, 0x1B, 0x17, 0xE5, 0xD3, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE,
    0x3D, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x5B, 0xD0, 0xAE, 0xF5, 0x47,
    0x34, 0x5B, 0x0D, 0x6C, 0xC2, 0x2A, 0xB3, 0x67, 0x5C, 0x80, 0xC9, 0x41, 0x0D, 0x35, 0x07, 0x92,
    0xF3, 0x50, 0x12, 0x15, 0xAC, 0x33, 0xEB, 0x2B, 0x8E, 0xBB, 0x72, 0x02, 0x21, 0x00, 0x8C, 0x81,
    0xB8, 0x27, 0x09, 0x40, 0x91, 0x18, 0xF2, 0xC5, 0x15, 0x73, 0x23, 0x60, 0x42, 0x81, 0xE3, 0x61,
    0x01, 0xE4, 0x58, 0x67, 0x0A, 0x33, 0xF5, 0xDC, 0xB4, 0x0E, 0xC1, 0x60, 0x83, 0x87
};

static const uint8_t g_challenge[32] = {
    0x0c, 0xa6, 0x34, 0xc8, 0x37, 0x2f, 0x87, 0x99, 0x99, 0x7e, 0x9e, 0xe9, 0xd5, 0xbc, 0x72, 0x71,
    0x84, 0xd1, 0x97, 0x0a, 0xea, 0xfe, 0xac, 0x60, 0x7e, 0xd1, 0x3e, 0x12, 0xb7, 0x32, 0x25, 0xf1
};
static const uint8_t g_response[64] = {
    0x2F, 0xA2, 0x13, 0x49, 0x31, 0x26, 0x4D, 0x68, 0x7C, 0x64, 0xA7, 0xC7, 0xE3, 0x82, 0x99, 0x5C,
    0xBD, 0xE9, 0x91, 0xBD, 0x8C, 0x0E, 0xB4, 0xFA, 0x36, 0x60, 0x06, 0x01, 0xBA, 0x04, 0x75, 0x7D,
    0x3B, 0xFA, 0xA0, 0x64, 0x0B, 0x27, 0xA3, 0x45, 0xD1, 0xC9, 0x07, 0xFE, 0x12, 0xFD, 0x9A, 0xF6,
    0xFF, 0x6E, 0x38, 0x64, 0xBE, 0xCA, 0x57, 0x60, 0xE1, 0x78, 0x95, 0x59, 0x73, 0x97, 0x03, 0x44
};
static const uint8_t g_public_key[64] = {
    0xCC, 0x20, 0xE3, 0xCC, 0x5E, 0xD8, 0x41, 0x19, 0x63, 0xC4, 0x5C, 0x72, 0x89, 0x6C, 0xC0, 0x53,
    0x21, 0x94, 0x6A, 0x2C, 0x3D, 0x45, 0x41, 0x6F, 0x5F, 0x2B, 0x1F, 0xC3, 0xD4, 0xB2, 0x0C, 0x26,
    0x96, 0xE9, 0x18, 0x4D, 0xB7, 0x0D, 0x23, 0xFB, 0xE6, 0x11, 0xEC, 0x5B, 0xFA, 0xFC, 0x29, 0x49,
    0xA8, 0x1E, 0x64, 0x61, 0xDE, 0x07, 0xA9, 0xBE, 0x0E, 0xF9, 0x2C, 0x30, 0x89, 0x24, 0x1E, 0x34
};

TEST_GROUP(atcacert_host_hw);

TEST_SETUP(atcacert_host_hw)
{
    int ret = atcab_init(gCfg);

    TEST_ASSERT_EQUAL(ATCA_SUCCESS, ret);
}

TEST_TEAR_DOWN(atcacert_host_hw)
{
    ATCA_STATUS status;

    status = atcab_wakeup();
    TEST_ASSERT_EQUAL(ATCA_SUCCESS, status);

    status = atcab_sleep();
    TEST_ASSERT_EQUAL(ATCA_SUCCESS, status);

    status = atcab_release();
    TEST_ASSERT_EQUAL(ATCA_SUCCESS, status);
}

TEST(atcacert_host_hw, atcacert_verify_cert_hw)
{
    int ret = 0;
    uint8_t signer_public_key[64];

    // Validate signer cert against its certificate authority (CA) public key
    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  g_signer_cert, sizeof(g_signer_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);

    // Get the signer's public key from its certificate
    ret = atcacert_get_subj_public_key(&g_test_cert_def_1_signer, g_signer_cert, sizeof(g_signer_cert), signer_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);

    // Validate the device cert against its certificate authority (CA) which is the signer
    ret = atcacert_verify_cert_hw(&g_test_cert_def_0_device, g_device_cert, sizeof(g_device_cert), signer_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);
}

TEST(atcacert_host_hw, atcacert_verify_cert_hw_verify_failed)
{
    int ret = 0;
    uint8_t bad_cert[sizeof(g_signer_cert)];
    size_t public_key_offset = g_test_cert_def_1_signer.std_cert_elements[STDCERT_PUBLIC_KEY].offset;

    memcpy(bad_cert, g_signer_cert, sizeof(bad_cert));

    // Change the cert public key to make the verify fail.
    bad_cert[public_key_offset]++;

    // Validate signer cert against its certificate authority (CA) public key
    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer, bad_cert, sizeof(bad_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_VERIFY_FAILED, ret);
}

TEST(atcacert_host_hw, atcacert_verify_cert_hw_short_cert)
{
    // Cert size is shortened so the TBS will run past the end of the cert
    int ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  g_signer_cert, sizeof(g_signer_cert) - 100, g_test_signer_1_ca_public_key);

    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_CERT, ret);
}

TEST(atcacert_host_hw, atcacert_verify_cert_hw_bad_sig)
{
    int ret = 0;
    uint8_t bad_cert[sizeof(g_signer_cert)];

    memcpy(bad_cert, g_signer_cert, sizeof(bad_cert));

    // Change the signature so it doesn't decode
    bad_cert[g_test_cert_def_1_signer.std_cert_elements[STDCERT_SIGNATURE].offset]++;

    // Validate signer cert against its certificate authority (CA) public key
    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  bad_cert, sizeof(bad_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_DECODING_ERROR, ret);
}

TEST(atcacert_host_hw, atcacert_verify_cert_hw_bad_params)
{
    int ret = 0;

    ret = atcacert_verify_cert_hw(NULL,  g_signer_cert, sizeof(g_signer_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  NULL, sizeof(g_signer_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(NULL,  NULL, sizeof(g_signer_cert), g_test_signer_1_ca_public_key);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  g_signer_cert, sizeof(g_signer_cert), NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(NULL,  g_signer_cert, sizeof(g_signer_cert), NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(&g_test_cert_def_1_signer,  NULL, sizeof(g_signer_cert), NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_cert_hw(NULL,  NULL, sizeof(g_signer_cert), NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);
}

TEST(atcacert_host_hw, atcacert_gen_challenge_hw)
{
    int ret = 0;
    uint8_t init[32];
    uint8_t challenge1[32];
    uint8_t challenge2[32];

    memset(init, 0, sizeof(init));
    memcpy(challenge1, init, sizeof(challenge1));
    memcpy(challenge2, init, sizeof(challenge2));

    ret = atcacert_gen_challenge_hw(challenge1);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);
    TEST_ASSERT(memcmp(init, challenge1, sizeof(init)) != 0);

    ret = atcacert_gen_challenge_hw(challenge2);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);
    TEST_ASSERT(memcmp(init, challenge2, sizeof(init)) != 0);

    TEST_ASSERT(memcmp(challenge1, challenge2, sizeof(challenge1)) != 0);
}

TEST(atcacert_host_hw, atcacert_gen_challenge_hw_bad_params)
{
    int ret = atcacert_gen_challenge_hw(NULL);

    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);
}

TEST(atcacert_host_hw, atcacert_verify_response_hw)
{
    int ret = 0;

    ret = atcacert_verify_response_hw(g_public_key, g_challenge, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_SUCCESS, ret);
}

TEST(atcacert_host_hw, atcacert_verify_response_hw_bad_challenge)
{
    int ret = 0;
    const uint8_t challenge[32] = {
        0x0d, 0xa6, 0x34, 0xc8, 0x37, 0x2f, 0x87, 0x99, 0x99, 0x7e, 0x9e, 0xe9, 0xd5, 0xbc, 0x72, 0x71,
        0x84, 0xd1, 0x97, 0x0a, 0xea, 0xfe, 0xac, 0x60, 0x7e, 0xd1, 0x3e, 0x12, 0xb7, 0x32, 0x25, 0xf1
    };

    ret = atcacert_verify_response_hw(g_public_key, challenge, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_VERIFY_FAILED, ret);
}

TEST(atcacert_host_hw, atcacert_verify_response_hw_bad_response)
{
    int ret = 0;
    const uint8_t response[64] = {
        0x30, 0xA2, 0x13, 0x49, 0x31, 0x26, 0x4D, 0x68, 0x7C, 0x64, 0xA7, 0xC7, 0xE3, 0x82, 0x99, 0x5C,
        0xBD, 0xE9, 0x91, 0xBD, 0x8C, 0x0E, 0xB4, 0xFA, 0x36, 0x60, 0x06, 0x01, 0xBA, 0x04, 0x75, 0x7D,
        0x3B, 0xFA, 0xA0, 0x64, 0x0B, 0x27, 0xA3, 0x45, 0xD1, 0xC9, 0x07, 0xFE, 0x12, 0xFD, 0x9A, 0xF6,
        0xFF, 0x6E, 0x38, 0x64, 0xBE, 0xCA, 0x57, 0x60, 0xE1, 0x78, 0x95, 0x59, 0x73, 0x97, 0x03, 0x44
    };

    ret = atcacert_verify_response_hw(g_public_key, g_challenge, response);
    TEST_ASSERT_EQUAL(ATCACERT_E_VERIFY_FAILED, ret);
}

TEST(atcacert_host_hw, atcacert_verify_response_hw_bad_public_key)
{
    int ret = 0;
    const uint8_t public_key[64] = {
        0x2F, 0xF3, 0xFD, 0x63, 0x14, 0x91, 0x8D, 0xAC, 0xA7, 0x47, 0x63, 0xC8, 0x63, 0x62, 0xA5, 0x8B,
        0x76, 0xAD, 0xCA, 0x5C, 0x6E, 0xDB, 0xB0, 0x93, 0x76, 0x0B, 0x0B, 0x83, 0xA6, 0x3A, 0x99, 0x8F,
        0x61, 0x10, 0xF8, 0x74, 0x34, 0x95, 0xCF, 0x33, 0x6F, 0xA4, 0xF1, 0xAB, 0xBD, 0xDE, 0x11, 0xB1,
        0xE2, 0x9E, 0x82, 0x8E, 0x8E, 0x78, 0x55, 0x32, 0x1D, 0x8D, 0x8C, 0xFA, 0x02, 0xDC, 0xCB, 0xD8
    };

    ret = atcacert_verify_response_hw(public_key, g_challenge, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_VERIFY_FAILED, ret);
}

TEST(atcacert_host_hw, atcacert_verify_response_hw_malformed_public_key)
{
    int ret = 0;
    const uint8_t public_key[64] = {
        0xCD, 0x20, 0xE3, 0xCC, 0x5E, 0xD8, 0x41, 0x19, 0x63, 0xC4, 0x5C, 0x72, 0x89, 0x6C, 0xC0, 0x53,
        0x21, 0x94, 0x6A, 0x2C, 0x3D, 0x45, 0x41, 0x6F, 0x5F, 0x2B, 0x1F, 0xC3, 0xD4, 0xB2, 0x0C, 0x26,
        0x96, 0xE9, 0x18, 0x4D, 0xB7, 0x0D, 0x23, 0xFB, 0xE6, 0x11, 0xEC, 0x5B, 0xFA, 0xFC, 0x29, 0x49,
        0xA8, 0x1E, 0x64, 0x61, 0xDE, 0x07, 0xA9, 0xBE, 0x0E, 0xF9, 0x2C, 0x30, 0x89, 0x24, 0x1E, 0x34
    };

    ret = atcacert_verify_response_hw(public_key, g_challenge, g_response);
    TEST_ASSERT_EQUAL(ATCA_EXECUTION_ERROR, ret); // Malformed public key results in an execution failure on the verify command
}

TEST(atcacert_host_hw, atcacert_verify_response_hw_bad_params)
{
    int ret = 0;

    ret = atcacert_verify_response_hw(NULL, g_challenge, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(g_public_key, NULL, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(NULL, NULL, g_response);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(g_public_key, g_challenge, NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(NULL, g_challenge, NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(g_public_key, NULL, NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);

    ret = atcacert_verify_response_hw(NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(ATCACERT_E_BAD_PARAMS, ret);
}
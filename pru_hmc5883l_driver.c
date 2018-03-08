/*
 * pru_hmc5883l_driver.c
 *
 *  Created on: 04 mar 2018
 *      Author: andrea
 */
#include <pru_i2c_driver.h>
#include "include/HMC5883L.h"
#include "include/pru_hmc5883l_driver.h"

#define HMC5883L_STATUS_DISABLED       0x00
#define HMC5883L_STATUS_ENABLED        0x01
#define HMC5883L_STATUS_TESTS_P1       0x02
#define HMC5883L_STATUS_TESTS_P2       0x04
#define HMC5883L_STATUS_TESTS_N1       0x08
#define HMC5883L_STATUS_TESTS_N2       0x10

typedef struct
{
    uint8_t detected;
    uint8_t initialized;
    uint8_t status;
    uint8_t regABack;
    uint8_t regBBack;
    uint8_t regModeBack;
    unsigned char* data;
    unsigned char* dataTests;
    uint8_t dataBytes;
    uint8_t dataTestsBytes;
    uint8_t (*callbackData)(uint8_t deviceNumber, uint8_t dataBytes,
                            unsigned char* data);
    uint8_t (*callbackDataTests)(uint8_t deviceNumber, uint8_t dataTestsBytes,
                                 unsigned char* dataTests);
} HMC5883LInfo;
// accept one device per channel
HMC5883LInfo hmc5883lList[2] = { { 1, HMC5883L_STATUS_DISABLED, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0 },
                                 { 2, HMC5883L_STATUS_DISABLED, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0 } };

uint8_t pru_hmc5883l_driver_Conf(uint8_t deviceNumber, HMC5883LConf* conf)
{
    hmc5883lList[deviceNumber - 1].data = conf->data;
    hmc5883lList[deviceNumber - 1].dataTests = conf->dataTests;
    hmc5883lList[deviceNumber - 1].callbackData = conf->callbackData;
    hmc5883lList[deviceNumber - 1].callbackDataTests = conf->callbackDataTests;
    return 1;
}
uint8_t pru_hmc5883l_driver_Enable(uint8_t deviceNumber)
{
    if (hmc5883lList[deviceNumber - 1].status != HMC5883L_STATUS_ENABLED)
    {
        if (pru_hmc5883l_driver_Detect(deviceNumber))
        {
            if (pru_hmc5883l_driver_Init(deviceNumber))
            {
                hmc5883lList[deviceNumber - 1].status = HMC5883L_STATUS_ENABLED;
                return 1;
            }
        }
        return 0;
    }
    return 1;
}
uint8_t pru_hmc5883l_driver_EnableAll()
{
    uint8_t result = 0;
    if (pru_hmc5883l_driver_Enable(1))
    {
        result += 1;
    }
    if (pru_hmc5883l_driver_Enable(2))
    {
        result += 2;
    }
    return result;
}
uint8_t pru_hmc5883l_driver_DisableAll()
{
    uint8_t result = 0;
    if (pru_hmc5883l_driver_Disable(1))
    {
        result += 1;
    }
    if (pru_hmc5883l_driver_Disable(2))
    {
        result += 2;
    }
    return result;
}

uint8_t pru_hmc5883l_driver_Disable(uint8_t deviceNumber)
{
    // TODO: spegnerlo ...
    if (hmc5883lList[deviceNumber - 1].status != HMC5883L_STATUS_DISABLED)
    {
        pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
        HMC5883L_RA_MODE,
                                0x01); // single mode
        hmc5883lList[deviceNumber - 1].detected = 0;
        hmc5883lList[deviceNumber - 1].initialized = 0;
        hmc5883lList[deviceNumber - 1].status = HMC5883L_STATUS_DISABLED;
    }
    return 1;
}
uint8_t pru_hmc5883l_driver_TestConnection(uint8_t deviceNumber)
{
    unsigned char payload[3] = { '\0', '\0', '\0' };
    if ((pru_i2c_driver_ReadBytes(deviceNumber, HMC5883L_ADDRESS,
    HMC5883L_RA_ID_A,
                                  3, payload) == 3)
            && ((payload[0] == 'H' && payload[1] == '4' && payload[2] == '3')))
    {
        return 3;
    }
    return 0;
}
uint8_t pru_hmc5883l_driver_Detect(uint8_t deviceNumber)
{
    if (!hmc5883lList[deviceNumber - 1].detected)
    {
        if (pru_hmc5883l_driver_TestConnection(1))
        {
            hmc5883lList[deviceNumber - 1].detected = 1;
            return 1;
        }
        return 0;
    }
    return 1;
}

uint8_t pru_hmc5883l_driver_DetectAll()
{
    uint8_t result = 0;
    if (pru_hmc5883l_driver_Detect(1))
    {
        result = 1;
    }
    if (pru_hmc5883l_driver_Detect(2))
    {
        result += 2;
    }
    return result;
}

uint8_t pru_hmc5883l_driver_Init(uint8_t deviceNumber)
{
    if (!hmc5883lList[deviceNumber - 1].initialized)
    {
        if (pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
        HMC5883L_RA_CONFIG_A,
                                    0x70)) // 8 sample, 15Hz, Normal measurement
        {
            if (pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
            HMC5883L_RA_CONFIG_B,
                                        0x20)) // ± 1.3 Ga // Gain 1090
            {
                if (pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
                HMC5883L_RA_MODE,
                                            0x00)) // continuous mode
                {
                    hmc5883lList[deviceNumber - 1].initialized = 1;
                    return 1;
                }
            }
        }
        return 0;
    }
    return 1;
}

uint8_t pru_hmc5883l_driver_InitAll()
{
    uint8_t result = 0;
    if (pru_hmc5883l_driver_Init(1))
    {
        result += 1;
    }
    if (pru_hmc5883l_driver_Init(2))
    {
        result += 2;
    }
    return result;
}

uint8_t pru_hmc5883l_driver_ReadData(uint8_t deviceNumber, unsigned char* data)
{
    if (!pru_hmc5883l_driver_Enable(deviceNumber))
    {
        return 0;
    }

    uint8_t result = pru_i2c_driver_ReadBytes(deviceNumber, HMC5883L_ADDRESS,
    HMC5883L_RA_DATAX_H,
                                              2, data);
    if (result == 2)
    {
        result += pru_i2c_driver_ReadBytes(deviceNumber, HMC5883L_ADDRESS,
        HMC5883L_RA_DATAY_H,
                                           2, data + 2);
        if (result == 4)
        {
            result += pru_i2c_driver_ReadBytes(deviceNumber, HMC5883L_ADDRESS,
            HMC5883L_RA_DATAZ_H,
                                               2, data + 4);
        }
    }
    if (result != 6)
    {
        result = 0;
    }
    return result;
}

uint8_t pru_hmc5883l_driver_SelfTests(uint8_t deviceNumber, unsigned char* data)
{
    if (!pru_hmc5883l_driver_Enable(deviceNumber))
    {
        return 0;
    }

    uint8_t RegModeCurrent;
    uint8_t RegACurrent;
    if (pru_i2c_driver_ReadReg(deviceNumber, HMC5883L_ADDRESS, HMC5883L_RA_MODE,
                               &RegModeCurrent))
    {
        if (pru_i2c_driver_ReadReg(deviceNumber, HMC5883L_ADDRESS,
        HMC5883L_RA_CONFIG_A,
                                   &RegACurrent))
        {
            if (pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
            HMC5883L_RA_CONFIG_A,
                                        0x12)) // positive test
            {
                if (pru_i2c_driver_WriteReg(deviceNumber, HMC5883L_ADDRESS,
                HMC5883L_RA_MODE,
                                            0x01)) // single mode
                {
                    {
                        uint8_t status;
                        while (1) // TODO: gestire con state machine e timeout
                        {
                            if (pru_i2c_driver_ReadReg(deviceNumber,
                            HMC5883L_ADDRESS,
                                                       HMC5883L_RA_STATUS,
                                                       &status))
                            {
                                if (!(status & 0x01))
                                {
                                    break;
                                }
                            }
                        }
                        while (1) // TODO: gestire con state machine e timeout
                        {
                            if (pru_i2c_driver_ReadReg(deviceNumber,
                            HMC5883L_ADDRESS,
                                                       HMC5883L_RA_STATUS,
                                                       &status))
                            {
                                if (status & 0x01)
                                {
                                    if (pru_hmc5883l_driver_ReadData(
                                            deviceNumber, data))
                                    {
                                        if (pru_i2c_driver_WriteReg(
                                                deviceNumber,
                                                HMC5883L_ADDRESS,
                                                HMC5883L_RA_CONFIG_A,
                                                RegACurrent))
                                        {
                                            if (pru_i2c_driver_WriteReg(
                                                    deviceNumber,
                                                    HMC5883L_ADDRESS,
                                                    HMC5883L_RA_MODE,
                                                    RegModeCurrent))
                                            {
                                                return 1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    return 0;
}

uint8_t pru_hmc5883l_driver_Pulse(uint8_t deviceNumber)
{
    if (hmc5883lList[deviceNumber - 1].status == HMC5883L_STATUS_ENABLED)
    {
        if (hmc5883lList[deviceNumber - 1].data != 0)
        {
            if (hmc5883lList[deviceNumber - 1].dataBytes =
                    pru_hmc5883l_driver_ReadData(
                            deviceNumber, hmc5883lList[deviceNumber - 1].data))
            {
                if (hmc5883lList[deviceNumber - 1].callbackData != 0)
                {
                    if ((*(hmc5883lList[deviceNumber - 1].callbackData))(
                            deviceNumber,
                            hmc5883lList[deviceNumber - 1].dataBytes,
                            hmc5883lList[deviceNumber - 1].data))
                    {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    return 1;
}

uint8_t pru_hmc5883l_driver_PulseAll()
{
    uint8_t result = 0;
    if (pulse(1))
    {
        result += 1;
    }
    if (pulse(2))
    {
        result += 2;
    }
    return result;
}

/*
 * Mimica del client:
 * 1) invoco Conf per configurare buffer data e callbacks
 * 2) invoco Enable (che provvede a testare la connessione e ad inizializzare il device)
 * 3) invoco pulse().
 * 4) nelle callback implemento l'invio dei dati tramite rpmsg.
 */

/*
 * pru_hmc5883l_driver.c
 *
 *  Created on: 04 mar 2018
 *      Author: andrea
 */
#include <pru_i2c_driver.h>
#include "include/HMC5883L.h"
#include "include/pru_hmc5883l_driver.h"

uint8_t pru_hmc5883l_driver_TestConnection()
{
    unsigned char payload[3] = {'\0', '\0', '\0'};
    if ((pru_i2c_driver_ReadBytes(HMC5883L_ADDRESS, HMC5883L_RA_ID_A, 3, payload) == 3)
            && ((payload[0] == 'H'
                    && payload[1] == '4'
                    && payload[2] == '3')))
    {
        return 3;
    }
    return 0;
}


uint8_t pru_hmc5883l_driver_InitHMC5883L()
{
    // Single Write
    uint8_t result = pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_CONFIG_A, 0x70);
    if (result > 0)
    {
        result = pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_CONFIG_B, 0x20);
        if (result > 0)
        {
            result = pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_MODE, 0x00);
        }
    }
    return result;
}

uint8_t pru_hmc5883l_driver_ReadHMC5883LData(unsigned char* data)
{
    uint8_t result = pru_i2c_driver_ReadBytes(HMC5883L_ADDRESS, HMC5883L_RA_DATAX_H, 2,
                               data);
    if (result == 2)
    {
        result += pru_i2c_driver_ReadBytes(HMC5883L_ADDRESS, HMC5883L_RA_DATAY_H, 2,
                            data + 2);
        if (result == 4)
        {
            result += pru_i2c_driver_ReadBytes(HMC5883L_ADDRESS, HMC5883L_RA_DATAZ_H, 2,
                                data + 4);
        }
    }
    if (result == 6)
    {
        return result;
    }
    else
    {
        return 0;
    }
}

uint8_t pru_hmc5883l_driver_SelfTestsHMC5883L(unsigned char* data)
{
    uint8_t RegModeCurrent;
    uint8_t RegACurrent;
    if (pru_i2c_driver_ReadReg(HMC5883L_ADDRESS, HMC5883L_RA_MODE, &RegModeCurrent))
    {
        if (pru_i2c_driver_ReadReg(HMC5883L_ADDRESS, HMC5883L_RA_CONFIG_A, &RegACurrent))
        {
            if (pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_CONFIG_A, 0x12)) // positive test
            {
                if (pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_MODE, 0x01)) // single mode
                {
                    {
                        uint8_t status;
                        while(1) {
                            if(pru_i2c_driver_ReadReg(HMC5883L_ADDRESS, HMC5883L_RA_STATUS, &status)) {
                                if(!(status & 0x01)) {
                                    break;
                                }
                            }
                        }
                        while(1) {
                            if(pru_i2c_driver_ReadReg(HMC5883L_ADDRESS, HMC5883L_RA_STATUS, &status)) {
                                if(status & 0x01) {
                                    if (pru_hmc5883l_driver_ReadHMC5883LData(data))
                                    {
                                        if (pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_CONFIG_A,
                                                     RegACurrent))
                                        {
                                            if (pru_i2c_driver_WriteReg(HMC5883L_ADDRESS, HMC5883L_RA_MODE,
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



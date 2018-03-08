/*
 * pru_hmc5883l_driver.h
 *
 *  Created on: 04 mar 2018
 *      Author: andrea
 */

#ifndef PRU_HMC5883L_DRIVER_H_
#define PRU_HMC5883L_DRIVER_H_

#include <stdint.h>

typedef struct {
    unsigned char* data;
    unsigned char* dataTests;
    uint8_t (*callbackData)(uint8_t deviceNumber, uint8_t dataBytes, unsigned char* data);
    uint8_t (*callbackDataTests)(uint8_t deviceNumber, uint8_t dataTestsBytes, unsigned char* dataTests);
} HMC5883LConf;

uint8_t pru_hmc5883l_driver_TestConnection(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_Init(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_InitAll();
uint8_t pru_hmc5883l_driver_Conf(uint8_t deviceNumber, HMC5883LConf* conf);
uint8_t pru_hmc5883l_driver_ReadData(uint8_t deviceNumber, unsigned char* data);
uint8_t pru_hmc5883l_driver_SelfTests(uint8_t deviceNumber, unsigned char* data);
uint8_t pru_hmc5883l_driver_Detect(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_DetectAll();
uint8_t pru_hmc5883l_driver_Enable(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_Disable(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_EnableAll();
uint8_t pru_hmc5883l_driver_DisableAll();
uint8_t pru_hmc5883l_driver_Pulse(uint8_t deviceNumber);
uint8_t pru_hmc5883l_driver_PulseAll();

#endif /* PRU_HMC5883L_DRIVER_H_ */

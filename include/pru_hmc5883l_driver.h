/*
 * pru_hmc5883l_driver.h
 *
 *  Created on: 04 mar 2018
 *      Author: andrea
 */

#ifndef PRU_HMC5883L_DRIVER_H_
#define PRU_HMC5883L_DRIVER_H_

#include <stdint.h>

uint8_t pru_hmc5883l_driver_TestConnection();
uint8_t pru_hmc5883l_driver_InitHMC5883L();
uint8_t pru_hmc5883l_driver_ReadHMC5883LData(unsigned char* data);
uint8_t pru_hmc5883l_driver_SelfTestsHMC5883L(unsigned char* data);



#endif /* PRU_HMC5883L_DRIVER_H_ */

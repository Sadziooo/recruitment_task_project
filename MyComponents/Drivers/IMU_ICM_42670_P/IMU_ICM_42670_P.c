/*
 * IMU_ICM_42670_P.c
 *
 *  Created on: Jan 26, 2025
 *      Author: Sadziooo
 */

// INCLUDES ---------------------------------------------------------------------------------------------------------------------------------

#include "IMU_ICM_42670_P.h"
#include <stddef.h>
#include <stdio.h>

// DEFINES -----------------------------------------------------------------------------------------------------------------------------------



// TYPEDEFS ---------------------------------------------------------------------------------------------------------------------------------



// VARIABLES --------------------------------------------------------------------------------------------------------------------------------

static IMU_Interface *i2c_interface = NULL;

// FUNCTION PROTOTYPES ---------------------------------------------------------------------------------------------------------------


// FUNCTIONS --------------------------------------------------------------------------------------------------------------------------------

int imu_init(IMU_Interface *i2c, uint8_t accel_mode, uint8_t gyro_mode, uint8_t idle, uint8_t accel_lp_clk_sel) {
	if (i2c == NULL) {
		return -1;
	}

	i2c_interface = i2c;
    uint8_t reserved = 0b00; // 

    if (accel_mode > 0b11) {
        return -2; // Wrong accel_mode
    }

    if (gyro_mode > 0b11 || gyro_mode == 10) {
        return -3; // Wrong gyro_mode
    }

    if (idle > 0b1) {
        return -4; // Wrong idle
    }

    if (accel_lp_clk_sel > 0b1) {
        return -5; // Wrong accel_lp_clk_sel
    }

	uint8_t pwr_mgmt0 = (accel_lp_clk_sel << 7) 
                        | (reserved << 5)
                        | (idle << 4) 
                        | (gyro_mode << 2) 
                        | accel_mode;

	return imu_write_register(IMU_REG_PWR_MGMT0, pwr_mgmt0);
}

int imu_config_accel(uint8_t range, uint8_t freq) {
    if (range > 0b11) {
        return -1;
    }

    if (freq < 0b0101 || freq > 0b1111) {
        return -2;
    }

    uint8_t pwr_mgmt0;
    int status = imu_read_register(IMU_REG_PWR_MGMT0, &pwr_mgmt0);
    if (status != 0) {
        return -3; 
    }

    uint8_t accel_mode = pwr_mgmt0 & 0x03;
    if (accel_mode == IMU_ACCEL_MODE_LP) {
        if (freq == IMU_ACCEL_ODR_800HZ_LN_MODE || freq == IMU_ACCEL_ODR_1_6KHZ_LN_MODE) {
            return -4;
        }
    }

    if (accel_mode == IMU_ACCEL_MODE_LN) {
        if (freq == IMU_ACCEL_ODR_3_125HZ_LP_MODE || freq == IMU_ACCEL_ODR_6_25HZ_LP_MODE || freq == IMU_ACCEL_ODR_1_5625HZ_LP_MODE) {
            return -4;
        }
    }

    uint8_t accel_config = (range << 6) | (freq & 0x0F);

    return imu_write_register(IMU_REG_ACCEL_CONFIG0, accel_config);
}

int imu_config_gyro(uint8_t range, uint8_t freq) {
    if (range > 0b11) {
        return -1;
    }

    if (freq < 0b0101 || freq > 0b1111) {
        return -2;
    }

	uint8_t gyro_config = (range << 6) | (freq & 0x0F);

	return imu_write_register(IMU_REG_GYRO_CONFIG0, gyro_config);
}

int imu_start_acquisition_default(void) {
	if(imu_start_acquisition_accel(0b11) != 0) {
        return -1;
    }

    if(imu_start_acquisition_gyro(0b11) != 0) {
        return -1;
    }

    return 0;
}

int imu_stop_acquisition_default(void) {
	if(imu_stop_acquisition_accel() != 0) {
        return -1;
    }

    if (imu_stop_acquisition_gyro() != 0){
        return -1;
    }
    
    return 0;
}

int imu_start_acquisition_accel(uint8_t mode) {
    uint8_t pwr_mgmt0;
    int status = imu_read_register(IMU_REG_PWR_MGMT0, &pwr_mgmt0);
    if (status != 0) {
        return -1; 
    }

    if (mode > 0b11 || mode == 0b10 || mode == 0b00)
    {
        return -2; 
    }

    pwr_mgmt0 |= mode;

	return imu_write_register(IMU_REG_PWR_MGMT0, pwr_mgmt0);
}

int imu_start_acquisition_gyro(uint8_t mode) {
    uint8_t pwr_mgmt0;
    int status = imu_read_register(IMU_REG_PWR_MGMT0, &pwr_mgmt0);
    if (status != 0) {
        return -1; 
    }

    if (mode > 0b11 || mode < 0b10)
    {
        return -2; 
    }
    
    pwr_mgmt0 |= mode;
	return imu_write_register(IMU_REG_PWR_MGMT0, pwr_mgmt0);
}

int imu_stop_acquisition_accel(void) {
    uint8_t pwr_mgmt0;
    int status = imu_read_register(IMU_REG_PWR_MGMT0, &pwr_mgmt0);
    if (status != 0) {
        return -1; 
    }

    pwr_mgmt0 &= 0xFC;
	return imu_write_register(IMU_REG_PWR_MGMT0, pwr_mgmt0);
}

int imu_stop_acquisition_gyro(void) {
    uint8_t pwr_mgmt0;
    int status = imu_read_register(IMU_REG_PWR_MGMT0, &pwr_mgmt0);
    if (status != 0) {
        return -1; 
    }

    pwr_mgmt0 &= 0xFC;
	return imu_write_register(IMU_REG_PWR_MGMT0, pwr_mgmt0);
}

int imu_read_accel(int16_t *accel_data) {
    if (accel_data == NULL) {
        return -1;
    }

    uint8_t raw_data[6];
    if (imu_read_multiple_registers(IMU_REG_ACCEL_DATA, raw_data, 6) != 0) {
        return -2;
    }

    accel_data[0] = (raw_data[0] << 8) | raw_data[1];    // X axis
    accel_data[1] = (raw_data[2] << 8) | raw_data[3];    // Y axis
    accel_data[2] = (raw_data[4] << 8) | raw_data[5];    // Z axis

    return 0;
}

int imu_read_gyro(int16_t *gyro_data) {
    if (gyro_data == NULL) {
        return -1;
    }

    uint8_t raw_data[6];
    if (imu_read_multiple_registers(IMU_REG_GYRO_DATA, raw_data, 6) != 0) {
        return -2;
    }

    gyro_data[0] = (raw_data[0] << 8) | raw_data[1];    // X axis
    gyro_data[1] = (raw_data[2] << 8) | raw_data[3];    // Y axis
    gyro_data[2] = (raw_data[4] << 8) | raw_data[5];    // Z axis
    
    return 0;
}

int imu_write_register(uint8_t reg_addr, uint8_t value) {
    return i2c_interface->write(IMU_I2C_ADDRESS, reg_addr, &value, 1);
}

int imu_read_register(uint8_t reg_addr, uint8_t *value) {
    return i2c_interface->read(IMU_I2C_ADDRESS, reg_addr, value, 1);
}

int imu_read_multiple_registers(uint8_t reg_addr, uint8_t *value, uint16_t len) {
    return i2c_interface->read(IMU_I2C_ADDRESS, reg_addr, value, len);
}

void imu_debug_registers(void) {
    uint8_t value;
    for (uint8_t reg = 0x0B; reg <= 0x39; reg++) {
        if (imu_read_register(reg, &value) == 0) {
            printf("Register 0x%02X: 0x%02X\n", reg, value);
        }
    }
}

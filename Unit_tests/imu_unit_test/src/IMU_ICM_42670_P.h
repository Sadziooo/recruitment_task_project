/*
 * IMU_ICM_42670_P.h
 *
 *  Created on: Jan 26, 2025
 *      Author: Sadziooo
 */

#ifndef DRIVERS_IMU_ICM_42670_P_IMU_ICM_42670_P_H_
#define DRIVERS_IMU_ICM_42670_P_IMU_ICM_42670_P_H_

// INCLUDES ---------------------------------------------------------------------------------------------------------------------------------

#include <stdint.h>

// DEFINES -----------------------------------------------------------------------------------------------------------------------------------
#define IMU_I2C_ADDRESS             0x68

// PWR_MGMT0
#define IMU_REG_PWR_MGMT0     		0x1F

#define IMU_ACCEL_MODE_TURN_OFF     0b00
#define IMU_ACCEL_MODE_LP           0b10 
#define IMU_ACCEL_MODE_LN           0b11 
#define IMU_GYRO_MODE_TURN_OFF      0b00
#define IMU_GYRO_MODE_STANDBY       0b01
#define IMU_GYRO_MODE_LN            0b11
#define IMU_IDLE_OFF                0b0
#define IMU_IDLE_ON                0b1
#define IMU_ACCEL_LP_CLK_SEL_WK_UP  0b0        
#define IMU_ACCEL_LP_CLK_SEL_RC_OSC 0b1 

// ACCEL_CONFIG0
#define IMU_REG_ACCEL_CONFIG0 	    0x21

// ACCEL_UI_FS_SEL (Full scale select for accelerometer UI interface output)
#define IMU_ACCEL_UI_FS_SEL_16g     0b00
#define IMU_ACCEL_UI_FS_SEL_8g      0b01
#define IMU_ACCEL_UI_FS_SEL_4g      0b10
#define IMU_ACCEL_UI_FS_SEL_2g      0b11

// ACCEL_ODR (Accelerometer ODR selection for UI interface output)
#define IMU_ACCEL_ODR_RESERVED_0000     0b0000  // Reserved
#define IMU_ACCEL_ODR_RESERVED_0001     0b0001  // Reserved
#define IMU_ACCEL_ODR_RESERVED_0010     0b0010  // Reserved
#define IMU_ACCEL_ODR_RESERVED_0011     0b0011  // Reserved
#define IMU_ACCEL_ODR_RESERVED_0100     0b0100  // Reserved
#define IMU_ACCEL_ODR_1_6KHZ_LN_MODE    0b0101  // 1.6 kHz (LN mode)
#define IMU_ACCEL_ODR_800HZ_LN_MODE     0b0110  // 800 Hz (LN mode)
#define IMU_ACCEL_ODR_400HZ_LP_LN_MODE  0b0111  // 400 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_200HZ_LP_LN_MODE  0b1000  // 200 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_100HZ_LP_LN_MODE  0b1001  // 100 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_50HZ_LP_LN_MODE   0b1010  // 50 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_25HZ_LP_LN_MODE   0b1011  // 25 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_12_5HZ_LP_LN_MODE 0b1100  // 12.5 Hz (LP or LN mode)
#define IMU_ACCEL_ODR_6_25HZ_LP_MODE    0b1101  // 6.25 Hz (LP mode)
#define IMU_ACCEL_ODR_3_125HZ_LP_MODE   0b1110  // 3.125 Hz (LP mode)
#define IMU_ACCEL_ODR_1_5625HZ_LP_MODE  0b1111  // 1.5625 Hz (LP mode)   

// GYRO_CONFIG0
#define IMU_REG_GYRO_CONFIG0  		0x20

// GYRO_UI_FS_SEL (Full scale select for gyroscope UI interface output)
#define IMU_GYRO_FS_2000DPS  0b00
#define IMU_GYRO_FS_1000DPS  0b01
#define IMU_GYRO_FS_500DPS   0b10
#define IMU_GYRO_FS_250DPS   0b11

// GYRO_ODR (Gyroscope ODR selection for UI interface output)
#define IMU_GYRO_ODR_RESERVED_0000  0b0000  // Reserved
#define IMU_GYRO_ODR_RESERVED_0001  0b0001  // Reserved
#define IMU_GYRO_ODR_RESERVED_0010  0b0010  // Reserved
#define IMU_GYRO_ODR_RESERVED_0011  0b0011  // Reserved
#define IMU_GYRO_ODR_RESERVED_0100  0b0100  // Reserved
#define IMU_GYRO_ODR_1_6KHZ         0b0101  // 1.6 kHz
#define IMU_GYRO_ODR_800HZ          0b0110  // 800 Hz
#define IMU_GYRO_ODR_400HZ          0b0111  // 400 Hz
#define IMU_GYRO_ODR_200HZ          0b1000  // 200 Hz
#define IMU_GYRO_ODR_100HZ          0b1001  // 100 Hz
#define IMU_GYRO_ODR_50HZ           0b1010  // 50 Hz
#define IMU_GYRO_ODR_25HZ           0b1011  // 25 Hz
#define IMU_GYRO_ODR_12_5HZ         0b1100  // 12.5 Hz
#define IMU_GYRO_ODR_RESERVED_1101  0b1101  // Reserved
#define IMU_GYRO_ODR_RESERVED_1110  0b1110  // Reserved
#define IMU_GYRO_ODR_RESERVED_1111  0b1111  // Reserved

#define IMU_REG_ACCEL_DATA    		0x0B
#define IMU_REG_GYRO_DATA     		0x11
#define IMU_REG_INT_STATUS    		0x39

// TYPEDEFS ---------------------------------------------------------------------------------------------------------------------------------

typedef struct {
    int (*write_imu)(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);
    int (*read_imu)(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);
} IMU_Interface_i;

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} accel_data_t;

typedef struct {
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
} gyro_data_t;

typedef struct {
	accel_data_t accel_data;
	gyro_data_t gyro_data;
} IMU_Data_t;

// VARIABLES --------------------------------------------------------------------------------------------------------------------------------



// FUNCTION PROTOTYPES ---------------------------------------------------------------------------------------------------------------

int imu_init(IMU_Interface_i *i2c, uint8_t accel_mode, uint8_t gyro_mode, uint8_t idle, uint8_t accel_lp_clk_sel);
int imu_config_accel(uint8_t range, uint8_t freq);
int imu_config_gyro(uint8_t range, uint8_t freq);

int imu_start_acquisition_default(void);
int imu_stop_acquisition_default(void);
int imu_start_acquisition_accel(uint8_t mode);
int imu_start_acquisition_gyro(uint8_t mode);
int imu_stop_acquisition_accel(void);
int imu_stop_acquisition_gyro(void);

int imu_check_data_ready_flag(void);
int imu_read_accel(int16_t *accel_data);
int imu_read_gyro(int16_t *gyro_data);

int imu_write_register(uint16_t reg_addr, uint8_t value);
int imu_read_register(uint16_t reg_addr, uint8_t *value);
int imu_read_multiple_registers(uint16_t reg_addr, uint8_t *value, uint16_t len);
void imu_debug_registers(void);


#endif /* DRIVERS_IMU_ICM_42670_P_IMU_ICM_42670_P_H_ */

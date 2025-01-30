#include <unity.h>
#include "IMU_ICM_42670_P.h"
#include "mock_IMU_ICM_42670_P.h"

void setUp(void);
void tearDown(void);

void setUp(void) {
    
}

void tearDown(void) {
    
}

void test_imu_init_null_interface(void) {
    int result = imu_init(NULL, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_imu_init_invalid_accel_mode(void) {
    IMU_Interface_i imu_interface;
    int result = imu_init(&imu_interface, 4, 0, 0, 0);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_imu_init_invalid_gyro_mode(void) {
    IMU_Interface_i imu_interface;
    int result = imu_init(&imu_interface, 0, 10, 0, 0);
    TEST_ASSERT_EQUAL(-3, result);
}

void test_imu_config_accel_invalid_range(void) {
    IMU_Interface_i imu_interface;
    int result = imu_config_accel(4, 0b1010);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_imu_config_accel_invalid_freq(void) {
    IMU_Interface_i imu_interface;
    int result = imu_config_accel(0b01, 0b0000);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_imu_read_accel_null_pointer(void) {
    int result = imu_read_accel(NULL);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_imu_read_accel_data_ready(void) {
    imu_check_data_ready_flag_ExpectAndReturn(1);
    int16_t accel_data[3];
    int result = imu_read_accel(accel_data);
    TEST_ASSERT_EQUAL(0, result);
}

void test_imu_read_accel_data_not_ready(void) {
    imu_check_data_ready_flag_ExpectAndReturn(0);
    int16_t accel_data[3];
    int result = imu_read_accel(accel_data);
    TEST_ASSERT_EQUAL(-2, result);
}

void test_imu_write_register(void) {
    IMU_Interface_i imu_interface;
    uint8_t value = 0xAB;
    
    imu_write_register_ExpectAndReturn(IMU_REG_PWR_MGMT0, value, 0);
    int result = imu_write_register(IMU_REG_PWR_MGMT0, value);
    TEST_ASSERT_EQUAL(0, result);
}

void test_imu_read_register(void) {
    IMU_Interface_i imu_interface;
    uint8_t value;
    
    imu_read_register_ExpectAndReturn(IMU_REG_PWR_MGMT0, &value, 0);
    int result = imu_read_register(IMU_REG_PWR_MGMT0, &value);
    TEST_ASSERT_EQUAL(0, result);
}

// int main(void) {
//     UNITY_BEGIN();

//     RUN_TEST(test_imu_init_null_interface);
//     RUN_TEST(test_imu_init_invalid_accel_mode);
//     RUN_TEST(test_imu_init_invalid_gyro_mode);
//     RUN_TEST(test_imu_config_accel_invalid_range);
//     RUN_TEST(test_imu_config_accel_invalid_freq);
//     RUN_TEST(test_imu_read_accel_null_pointer);
//     RUN_TEST(test_imu_read_accel_data_ready);
//     RUN_TEST(test_imu_read_accel_data_not_ready);
//     RUN_TEST(test_imu_write_register);
//     RUN_TEST(test_imu_read_register);

//     return UNITY_END();
// }
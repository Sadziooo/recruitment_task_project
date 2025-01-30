#include <unity.h>
#include "IMU_ICM_42670_P.h"
#include "mock_IMU_ICM_42760_P.h"

// Prototypy funkcji pomocniczych
void setUp(void);
void tearDown(void);

// Inicjalizacja mocków
void setUp(void) {
    // Resetowanie mocków przed każdym testem
    mock_IMU_ICM_42670_P_Init();
}

void tearDown(void) {
    // Sprzątanie po testach
    mock_IMU_ICM_42670_P_Verify();
}

// Testowanie funkcji imu_init
void test_imu_init_null_interface(void) {
    int result = imu_init(NULL, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL(-1, result);  // Sprawdź, czy zwraca -1, gdy interfejs jest NULL
}

void test_imu_init_invalid_accel_mode(void) {
    IMU_Interface_i imu_interface;
    int result = imu_init(&imu_interface, 4, 0, 0, 0);
    TEST_ASSERT_EQUAL(-2, result);  // Sprawdź, czy zwraca -2, gdy accel_mode jest niepoprawny
}

void test_imu_init_invalid_gyro_mode(void) {
    IMU_Interface_i imu_interface;
    int result = imu_init(&imu_interface, 0, 10, 0, 0);
    TEST_ASSERT_EQUAL(-3, result);  // Sprawdź, czy zwraca -3, gdy gyro_mode jest niepoprawny
}

// Testowanie funkcji imu_config_accel
void test_imu_config_accel_invalid_range(void) {
    IMU_Interface_i imu_interface;
    int result = imu_config_accel(4, 0b1010);
    TEST_ASSERT_EQUAL(-1, result);  // Sprawdź, czy zwraca -1, gdy zakres jest niepoprawny
}

void test_imu_config_accel_invalid_freq(void) {
    IMU_Interface_i imu_interface;
    int result = imu_config_accel(0b01, 0b0000);
    TEST_ASSERT_EQUAL(-2, result);  // Sprawdź, czy zwraca -2, gdy częstotliwość jest niepoprawna
}

// Testowanie funkcji imu_read_accel
void test_imu_read_accel_null_pointer(void) {
    int result = imu_read_accel(NULL);
    TEST_ASSERT_EQUAL(-1, result);  // Sprawdź, czy zwraca -1, gdy wskaźnik do danych jest NULL
}

void test_imu_read_accel_data_ready(void) {
    // Mockowanie funkcji sprawdzającej flagę gotowości danych
    imu_check_data_ready_flag_ExpectAndReturn(1);  // Zakłada, że flaga jest ustawiona
    int16_t accel_data[3];
    int result = imu_read_accel(accel_data);
    TEST_ASSERT_EQUAL(0, result);  // Powinno zwrócić 0, jeśli dane zostały poprawnie odczytane
}

void test_imu_read_accel_data_not_ready(void) {
    // Mockowanie funkcji sprawdzającej flagę gotowości danych
    imu_check_data_ready_flag_ExpectAndReturn(0);  // Zakłada, że flaga nie jest ustawiona
    int16_t accel_data[3];
    int result = imu_read_accel(accel_data);
    TEST_ASSERT_EQUAL(-2, result);  // Powinno zwrócić -2, jeśli dane nie są gotowe
}

// Testowanie funkcji imu_write_register
void test_imu_write_register(void) {
    IMU_Interface_i imu_interface;
    uint8_t value = 0xAB;
    
    // Mockowanie operacji zapisu
    imu_write_register_ExpectAndReturn(IMU_REG_PWR_MGMT0, value, 0);  // Spodziewany wynik: 0 (brak błędu)
    int result = imu_write_register(IMU_REG_PWR_MGMT0, value);
    TEST_ASSERT_EQUAL(0, result);  // Powinno zwrócić 0, jeśli zapis był udany
}

// Testowanie funkcji imu_read_register
void test_imu_read_register(void) {
    IMU_Interface_i imu_interface;
    uint8_t value;
    
    // Mockowanie operacji odczytu
    imu_read_register_ExpectAndReturn(IMU_REG_PWR_MGMT0, &value, 0);  // Spodziewany wynik: 0 (brak błędu)
    int result = imu_read_register(IMU_REG_PWR_MGMT0, &value);
    TEST_ASSERT_EQUAL(0, result);  // Powinno zwrócić 0, jeśli odczyt był udany
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_imu_init_null_interface);
    RUN_TEST(test_imu_init_invalid_accel_mode);
    RUN_TEST(test_imu_init_invalid_gyro_mode);
    RUN_TEST(test_imu_config_accel_invalid_range);
    RUN_TEST(test_imu_config_accel_invalid_freq);
    RUN_TEST(test_imu_read_accel_null_pointer);
    RUN_TEST(test_imu_read_accel_data_ready);
    RUN_TEST(test_imu_read_accel_data_not_ready);
    RUN_TEST(test_imu_write_register);
    RUN_TEST(test_imu_read_register);

    return UNITY_END();
}
/*
 * imu_simulator.c
 *
 *  Created on: Jan 27, 2025
 *      Author: Sadziooo
 */

// INCLUDES ---------------------------------------------------------------------------------------------------------------------------------

#include "imu_simulator.h"
#include "IMU_ICM_42670_P.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/sys.h"
#include "sys/socket.h"
#include "ip.h"


#include <string.h>
#include <stdbool.h>

// DEFINES -----------------------------------------------------------------------------------------------------------------------------------

#define SIMULATOR_IP0 192
#define SIMULATOR_IP1 168
#define SIMULATOR_IP2 0
#define SIMULATOR_IP3 121
#define SIMULATOR_PORT 12345

#define I2C_OP_READ  0x01
#define I2C_OP_WRITE 0x02

#define MAX_I2C_DATA_LENGTH 256

// TYPEDEFS ---------------------------------------------------------------------------------------------------------------------------------

typedef struct __attribute__((__packed__)) {
	uint8_t function_code;
	uint16_t device_address;
	uint16_t register_address;
	uint16_t data_length;
} i2c_frame_header_t;

typedef struct __attribute__((__packed__)){
	i2c_frame_header_t i2c_frame_header;
    uint8_t data[MAX_I2C_DATA_LENGTH];
} i2c_frame_t;

// FUNCTION PROTOTYPES ---------------------------------------------------------------------------------------------------------------

int my_i2c_write(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t size);
int my_i2c_read(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t size);
static int udp_send_receive(i2c_frame_t *frame, size_t frame_size, uint8_t *buffer, size_t buffer_size);
void prepare_i2c_frame(i2c_frame_t *frame, uint8_t function_code, uint16_t device_address, uint16_t register_address, uint16_t data_length, uint8_t *data);
void udp_send_hello(void);

// VARIABLES --------------------------------------------------------------------------------------------------------------------------------

IMU_Interface_i imu_interface = {
    .write_imu = my_i2c_write,
    .read_imu = my_i2c_read
};

IMU_Data_t imu_data;
i2c_frame_t frame;

osThreadId_t imu_simulator_task_handle;
osThreadAttr_t imu_simulator_task_attributes = {
  .name = "imu_simulator_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

// FUNCTIONS --------------------------------------------------------------------------------------------------------------------------------

extern volatile bool lwip_is_ready_flag;

#define BUFFER_SIZE 64

void imu_simulator_task(void *argument) {
	while(!lwip_is_ready_flag) {
		osDelay(1);
	}

//	imu_init(&imu_interface, IMU_ACCEL_MODE_LN, IMU_GYRO_MODE_LN, IMU_IDLE_OFF, IMU_ACCEL_LP_CLK_SEL_WK_UP);
//	imu_config_accel(IMU_ACCEL_UI_FS_SEL_4g, IMU_ACCEL_ODR_200HZ_LP_LN_MODE);
//	imu_config_gyro(IMU_GYRO_FS_250DPS, IMU_GYRO_ODR_200HZ);
//	imu_start_acquisition_default();

	uint8_t recv_buffer[BUFFER_SIZE];
    const char message[] = "Hello from stm32!";
	prepare_i2c_frame(&frame, 0x01, 0x86, 0x03, strlen(message), (uint8_t *)message);

    for(;;) {
//    	if(imu_read_accel((int16_t *)&imu_data.accel_data)) {
//            continue;
//        }
//   	    osDelay(100);
//        if(imu_read_gyro((int16_t *)&imu_data.gyro_data)) {
//            continue;
//        }

//    	udp_send_hello();
//    	int received_bytes = udp_send_rcv(recv_buffer, BUFFER_SIZE);
    	int received_bytes = udp_send_receive(&frame, sizeof(frame.i2c_frame_header) + frame.i2c_frame_header.data_length, recv_buffer, BUFFER_SIZE);

        osDelay(3000);
    }
}

void udp_send_hello(void) {
    struct netconn *conn;
    struct netbuf *buf;
    ip_addr_t server_ip;
    err_t err;

    conn = netconn_new(NETCONN_UDP);
    if (conn == NULL) {
        printf("Error: Unable to create netconn\n");
        return;
    }

    IP4_ADDR(&server_ip, SIMULATOR_IP0, SIMULATOR_IP1, SIMULATOR_IP2, SIMULATOR_IP3);

    err = netconn_connect(conn, &server_ip, SIMULATOR_PORT);
    if (err != ERR_OK) {
        printf("Error: Unable to connect to server\n");
        netconn_delete(conn);
        return;
    }

    buf = netbuf_new();
    if (buf == NULL) {
        printf("Error: Unable to create netbuf\n");
        netconn_delete(conn);
        return;
    }

    const char *message = "Hello from stm32!";
    netbuf_ref(buf, message, strlen(message));

    err = netconn_send(conn, buf);
    if (err != ERR_OK) {
        printf("Error: Failed to send data\n");
    } else {
        printf("Message sent successfully\n");
    }

    netbuf_delete(buf);
    netconn_delete(conn);
}

// ----------------------------------------------------------------------------------------------------------------------------

static int udp_send_receive(i2c_frame_t *frame, size_t frame_size, uint8_t *buffer, size_t buffer_size) {
    err_t err;
    struct netconn *conn;
    struct netbuf *send_buf;
    struct netbuf *recv_buf;
    ip_addr_t ipaddr;
    size_t recv_len = 0;

    IP4_ADDR(&ipaddr, SIMULATOR_IP0, SIMULATOR_IP1, SIMULATOR_IP2, SIMULATOR_IP3);

    conn = netconn_new(NETCONN_UDP);
    if (conn == NULL) {
        return -1;
    }

    err = netconn_bind(conn, IP_ADDR_ANY, 54321);
    if (err != ERR_OK) {
        netconn_delete(conn);
        return -1;
    }

    send_buf = netbuf_new();
    if (send_buf == NULL) {
        netconn_delete(conn);
        return -1;
    }

    if (netbuf_ref(send_buf, frame, frame_size) != ERR_OK) {
        netbuf_delete(send_buf);
        netconn_delete(conn);
        return -1;
    }

    err = netconn_sendto(conn, send_buf, &ipaddr, SIMULATOR_PORT);
    netbuf_delete(send_buf);
    if (err != ERR_OK) {
        netconn_delete(conn);
        return -1;
    }

    netconn_set_recvtimeout(conn, 1000);

    err = netconn_recv(conn, &recv_buf);
    if (err != ERR_OK) {
        netconn_delete(conn);
        return -1;
    }

    recv_len = netbuf_len(recv_buf);
    if (recv_len > buffer_size) {
        recv_len = buffer_size;
    }

    netbuf_copy(recv_buf, buffer, recv_len);

    netbuf_delete(recv_buf);
    netconn_delete(conn);

    return recv_len; // Zwracamy liczbę odebranych bajtów
}

void prepare_i2c_frame(i2c_frame_t *frame, uint8_t function_code, uint16_t device_address, uint16_t register_address, uint16_t data_length, uint8_t *data) {
    frame->i2c_frame_header.function_code = function_code;
    frame->i2c_frame_header.device_address = device_address;
    frame->i2c_frame_header.register_address = register_address;
    frame->i2c_frame_header.data_length = data_length;
    memcpy(frame->data, data, data_length);
}


int my_i2c_read(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t size) {
    if (size > MAX_I2C_DATA_LENGTH) {
        return -1;
    }

    i2c_frame_t frame = {0};
    frame.i2c_frame_header.function_code = I2C_OP_READ;
    frame.i2c_frame_header.device_address = htons(dev_addr);
    frame.i2c_frame_header.register_address = reg_addr;
    frame.i2c_frame_header.data_length = htons(size);

    uint8_t buffer[sizeof(i2c_frame_t) + MAX_I2C_DATA_LENGTH];

    // Wysyłanie ramki i odbieranie odpowiedzi
    int received = udp_send_receive(&frame, sizeof(i2c_frame_t) - sizeof(frame.data), buffer, sizeof(buffer));
    if (received < 0) {
        return -1;
    }

    i2c_frame_t *response_frame = (i2c_frame_t *)buffer;
    uint16_t response_data_length = ntohs(response_frame->i2c_frame_header.data_length);

    if (response_data_length > size || received < sizeof(i2c_frame_t) - sizeof(frame.data) + response_data_length) {
        return -1;
    }

    memcpy(data, response_frame->data, response_data_length);
    return 0;
}

int my_i2c_write(uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t size) {
    if (size > MAX_I2C_DATA_LENGTH) {
        return -1;
    }

    i2c_frame_t frame = {0};
    frame.i2c_frame_header.function_code = I2C_OP_WRITE;
    frame.i2c_frame_header.device_address = htons(dev_addr);
    frame.i2c_frame_header.register_address = reg_addr;
    frame.i2c_frame_header.data_length = htons(size);
    memcpy(frame.data, data, size);

    // Wysyłanie ramki
    int sent = udp_send_receive(&frame, sizeof(i2c_frame_t) - sizeof(frame.data) + size, NULL, 0);
    return (sent == 0) ? 0 : -1;
}


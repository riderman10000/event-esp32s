#include <stdio.h>
#include "my_serial.hpp"

#define UART_PORT_NUM      UART_NUM_1 // Use UART1 (ESP32-S3 has UART0, UART1, and UART2)
#define UART_BAUD_RATE     115200     // Baud rate for serial communication
#define TX_PIN             17         // Pin for UART TX (GPIO17 is for TX)
#define RX_PIN             16         // Pin for UART RX (GPIO16 is for RX)
#define BUF_SIZE           1024       // Size of buffer to store incoming data

static const char* SERIAL_TAG = "My Serial";

// class MySerial{
void MySerial::begin(){
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    // Install UART driver and set buffer size for incoming data
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0));
    
    // Apply UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));

    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(SERIAL_TAG, "UART initialized successfully");
}


void MySerial::send_data(const char* data){
    // send data over the UART
    uart_write_bytes(UART_PORT_NUM, data, strlen(data));
}

void MySerial::receive_data(){
    uint8_t data[BUF_SIZE];
    int length = 0;
    while (1) {
        // Read data from UART
        length = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(1000));
        if (length > 0) {
            data[length] = '\0'; // Null terminate the data received
            ESP_LOGI(SERIAL_TAG, "Received: %s", data);  // Print received data
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second between reads
    }
}
// };

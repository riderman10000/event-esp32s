#ifndef MY_SERIAL_HPP 
#define MY_SERIAL_HPP

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "string.h"

#define UART_PORT_NUM      UART_NUM_1 // Use UART1 (ESP32-S3 has UART0, UART1, and UART2)
#define UART_BAUD_RATE     115200     // Baud rate for serial communication
#define TX_PIN             17         // Pin for UART TX (GPIO17 is for TX)
#define RX_PIN             16         // Pin for UART RX (GPIO16 is for RX)
#define BUF_SIZE           1024       // Size of buffer to store incoming data

class MySerial{
public:
    void begin();
    // void begin(
    //     int baud_rate = UART_BAUD_RATE, int tx = TX_PIN, int rx = RX_PIN);
    void send_data(const char* data);
    void receive_data();
};

#endif 
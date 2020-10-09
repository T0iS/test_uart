/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <strings.h>
#include <signal.h>

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#define ECHO_TEST_TXD (GPIO_NUM_4)
#define ECHO_TEST_RXD (GPIO_NUM_5)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define BUF_SIZE (8000)

void catch_sig(int sig)
{

    uart_driver_delete(UART_NUM_1);
    exit(1);
}

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,

    };
    uart_driver_install(UART_NUM_1, 256, 256, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        // Read data from the UART
        if (uart_read_bytes(UART_NUM_1, data, 256, 1000 / portTICK_RATE_MS))
        {
            printf("%s", data);
            bzero(&data, sizeof(data));
        }
        // Write data back to the UART
        //uart_write_bytes(UART_NUM_1, (const char *) data, len);
    }
}

void app_main(void)
{
    /*struct sigaction siga;
    bzero(&siga, sizeof(siga));
    siga.sa_handler = catch_sig;
    sigemptyset(&siga.sa_mask);
    siga.sa_flags = 0;

    // catch sig <CTRL-C>
    sigaction(SIGINT, &siga, NULL);
    
    */
   xTaskCreate(echo_task, "uart_echo_task", BUF_SIZE, NULL, 10, NULL);
}

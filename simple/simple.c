
/* Very basic example that just demonstrates we can run at all!
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"

#include "ssid_config.h"
#include "easyq_config.h"

#include "easyq.h"

#define STATUS_QUEUE "status"
#define COMMAND_QUEUE "cmd"

EQSession * eq;


void sender(void* args) {
    err_t err;
    int c = 0;
    char buff[16];
    Queue * queue = Queue_new(STATUS_QUEUE);
    while (1) {
        printf("Inititlizing easyq\n");
        delay(100);
        err = easyq_init(&eq);
        if (err != ERR_OK) {
            printf("Cannot Inititalize the easyq\n");
            delay(1000);
            continue;
        }
        printf("Session ID: %s\n", eq->id);
        while (1) {
            sprintf(buff, "%08d", c);
            //easyq_push(eq, queue, buff, -1);
            delay(500);
            c++;
        }
    }
}


/*
void listener(void* args) {
    err_t err;
    char * buff;
    size_t buff_len;
    Queue * queue = Queue_new(COMMAND_QUEUE);
    while (1) {
        delay(500);
        if (!eq) {
            continue;
        }
        err = easyq_pull(eq, queue);
        if (err != ERR_OK) {
            continue;
        }
        while(1) {
            err = easyq_read(eq, &buff, &buff_len);
            if (err != ERR_OK) {
                printf("Error reading from EasyQ");
                continue;
            }
            printf("-> %s", buff);
        }
    }
}
*/

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    // Wifi
    struct sdk_station_config config = {
        .ssid     = WIFI_SSID,
        .password = WIFI_PASS,
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    xTaskCreate(sender, "sender", 512, NULL, 2, NULL);
    //xTaskCreate(listener, "listener", 384, NULL, 2, NULL);
}


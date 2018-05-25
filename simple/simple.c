
/* Very basic example that just demonstrates we can run at all!
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ssid_config.h"
#include "lwip/api.h"
#include "string.h"


#define EASYQ_HOST "192.168.8.44"
#define EASYQ_PORT 1085


void check_wifi_connection()
{
    uint8_t status = sdk_wifi_station_get_connect_status();
    while (status != STATION_GOT_IP)
    {
        status = sdk_wifi_station_get_connect_status();
        vTaskDelay(1000);
        switch (status)
        {
        case STATION_WRONG_PASSWORD:
            printf("WiFi: wrong password\n\r");
            break;
        case STATION_NO_AP_FOUND:
            printf("WiFi: AP not found\n\r");
            break;
        case STATION_CONNECT_FAIL:
            printf("WiFi: connection failed\r\n");
            break;
        case STATION_GOT_IP:
            break;
        default:
            printf("%s: status = %d\n\r", __func__, status);
            break;
        }
    }
}


netconn* connect(const char* username, const char* session_id) {
	int err;

	printf("Waiting for wifi connection\n");
	check_wifi_connection();	
	
	// Parsing the address
	ip_addr_t remote_addr;
	if (!ipaddr_aton(EASYQ_HOST, &remote_addr)) {
		printf("Invalid IP Address: %s,  %s, (%s)\n", EASYQ_HOST, __FUNCTION__, lwip_strerr(err));
		return;
	}

	// Creating the connection
	struct netconn* conn;
	int local_port = EASYQ_PORT-10;
	conn = netconn_new(NETCONN_TCP);
    do {
        err = netconn_bind(conn, IP_ADDR_ANY, ++local_port);
    } while(err == ERR_USE && local_port < EASYQ_PORT + 10);

    if(err != ERR_OK) {
        netconn_delete(conn);
        printf("%s : Could not bind! (%s)\n", __FUNCTION__, lwip_strerr(err));
        return;
    }
	printf("Local port: %d\n", local_port);
	
	// Connecting to the remote host
	err = netconn_connect(conn, &remote_addr, (u16_t)EASYQ_PORT);
	if (err != ERR_OK) {
	    netconn_delete(conn);
		printf("Could not connect: %s:%d\n", EASYQ_HOST, EASYQ_PORT);
		return, session_id;
	}
	printf("Connected to: %s:%d\n", EASYQ_HOST, EASYQ_PORT);

	// Login
	const char* greeting = "LOGIN Adam;\n";
	err = netconn_write(conn, greeting, strlen(greeting), NETCONN_COPY); 
	if (err != ERR_OK) {
	    netconn_delete(conn);
		printf("Could not write: %s:%d\n", EASYQ_HOST, EASYQ_PORT);
	}

	err = netconn_recv
	return conn;
}

void task1(void *pvParameters)
{
	const char* session_id;
	struct netconn* conn = connect("Adam", session_id);
	printf("Session Id: %s", session_id);
	
    QueueHandle_t *queue = (QueueHandle_t *)pvParameters;
    printf("Hello from task1!\r\n");
    uint16_t count = 0;
    while(1) {
        vTaskDelay(3000);
		count = sdk_system_adc_read();
        xQueueSend(*queue, &count, 0);
    }
}


void task2(void *pvParameters)
{
    printf("Hello from task 2!\r\n");
    QueueHandle_t *queue = (QueueHandle_t *)pvParameters;
    while(1) {
        uint32_t count;
        if(xQueueReceive(*queue, &count, 3000)) {
            printf("Got %u\n", count);
        } else {
            printf("No msg :(\n");
        }
    }
}


static QueueHandle_t mainqueue;

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

    mainqueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(task1, "tsk1", 512, &mainqueue, 2, NULL);
    xTaskCreate(task2, "tsk2", 256, &mainqueue, 2, NULL);
}

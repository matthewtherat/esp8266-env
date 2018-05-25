
/* Very basic example that just demonstrates we can run at all!
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ssid_config.h"
#include "string.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#define LWIP_SO_RCVTIMEO 1
#define LWIP_SOCKET 1
#define EASYQ_HOST "192.168.8.44"
#define EASYQ_PORT "1085"
#define EASYQ_LOGIN "esp8266-01"
#define COMMAND_QUEUE "cmd"
#define STATUS_QUEUE "status"
#define delay() vTaskDelay(500 / portTICK_PERIOD_MS)
#define delay_continue() delay();continue;

int socket_id = -1;
char session_id[32];

void wait_for_wifi_connection()
{
    uint8_t status = sdk_wifi_station_get_connect_status();
    while (status != STATION_GOT_IP)
    {
        status = sdk_wifi_station_get_connect_status();
        delay();
        switch (status)
        {
        case STATION_WRONG_PASSWORD:
            printf("WiFi: wrong password\n");
            break;
        case STATION_NO_AP_FOUND:
            printf("WiFi: AP not found\n");
            break;
        case STATION_CONNECT_FAIL:
            printf("WiFi: connection failed\n");
            break;
        case STATION_GOT_IP:
            break;
        default:
            printf("%s: status = %d\n", __func__, status);
            break;
        }
    }
}


void listener_task(void *pvParameters)
{
    static char recv_buf[96];
    int r = 0;
	printf("Hello from task 2!\n");
    while(1) {
		if (socket_id < 0 || session_id == NULL) {
			delay_continue();
		}

		const char* pull = "PULL FROM "COMMAND_QUEUE";\n";
        if (lwip_write(socket_id, pull, strlen(pull)) < 0) {
            printf("... socket send failed\n");
			delay_continue();
        }

		while (1) {
    		bzero(recv_buf, 96);
    		r = lwip_recv(socket_id, recv_buf, 96, 0);
    		if (r < 0){
    			printf("Error Reading");
    			break;
    		}
    		printf("%s", recv_buf);
			delay();
		}
		delay_continue();
    }
}


void easyq_task(void *pvParameters)
{
	const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;

	// Wait for wifi conection
    wait_for_wifi_connection();

    while(1) {

        printf("Running DNS lookup for %s...\n", EASYQ_HOST);
        int err = getaddrinfo(EASYQ_HOST, EASYQ_PORT, &hints, &res);

        if (err != 0 || res == NULL) {
            printf("DNS lookup failed err=%d res=%p\n", err, res);
            if(res)
                freeaddrinfo(res);
			delay();
            continue;
        }

        struct sockaddr *sa = res->ai_addr;
        if (sa->sa_family == AF_INET) {
            printf("DNS lookup succeeded. IP=%s\n", inet_ntoa(((struct sockaddr_in *)sa)->sin_addr));
        }

        socket_id = socket(res->ai_family, res->ai_socktype, 0);
        if(socket_id < 0) {
            printf("... Failed to allocate socket.\n");
            freeaddrinfo(res);
			delay();
            continue;
        }

        printf("... allocated socket\n");

        if(connect(socket_id, res->ai_addr, res->ai_addrlen) != 0) {
            lwip_close(socket_id);
            freeaddrinfo(res);
            printf("... socket connect failed.\n");
			delay();
            continue;
        }
		int timeout=400;
		err = lwip_setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));
		LWIP_ASSERT("err == 0", err == 0);

        printf("... connected\n");
        freeaddrinfo(res);

        char *req = "LOGIN "EASYQ_LOGIN";\n";
        if (lwip_write(socket_id, req, strlen(req)) < 0) {
            printf("... socket send failed\n");
            lwip_close(socket_id);
			delay();
            continue;
        }
        printf("... socket send success\n");

        int r = 0;
		printf("Reading from socket: r: %d\n", r);
		r = lwip_recv(socket_id, session_id, 32, 0);
		printf("Done Reading from socket: r: %d\n", r);
		if(r > 0) {
		    printf("%s", session_id);
		}
		else if (r < 0){
			printf("error Reading");
			lwip_close(socket_id);
			continue;
		}
		printf("Starting new loop r: %d\n", r);

		uint16_t voltage;
		size_t msg_len;
		char data[64];
		while(1) {
			voltage = sdk_system_adc_read();
			msg_len = sprintf(data, "PUSH %d INTO "STATUS_QUEUE";\n", voltage);
			if (lwip_write(socket_id, data, msg_len) < 0) {
				printf(".... Socket send failed\n");
				break;
			}
			delay();
		}
        lwip_close(socket_id);
        printf("\nStarting again!\n");
    }
	vTaskDelete( NULL );
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
    xTaskCreate(easyq_task, "sender", 512, &mainqueue, 2, NULL);
	xTaskCreate(listener_task, "listener", 384, &mainqueue, 2, NULL);
}

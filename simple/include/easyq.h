#include "string.h"
#include "stdlib.h"

#include "lwip/api.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/err.h"

#include "easyq_config.h"
#include "common.h"


#define LWIP_TCPIP_CORE_LOCKING 1
#define LWIP_SOCKET 1

#ifndef EASYQ_PORT
#define EASYQ_PORT "1085"
#endif

#ifndef EASYQ_READ_BUFFER_SIZE
#define EASYQ_READ_BUFFER_SIZE 1024
#endif


typedef struct EQSession {
	char * id;
    int socket;
    bool ready;
    char * readbuffer;
} EQSession;

typedef struct Queue {
    char * name;
    size_t len;
} Queue;

err_t easyq_init(EQSession ** session);
void easyq_close(EQSession * session);

err_t easyq_read(EQSession * session, char ** line, size_t * len);
err_t easyq_write(EQSession * session, char * line, size_t len);
err_t easyq_push(EQSession * session, Queue * queue, char * msg, size_t len);
err_t easyq_pull(EQSession * session, Queue * queue);

Queue * Queue_new(char * name);


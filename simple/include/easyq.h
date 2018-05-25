#include "lwip/api.h"


typedef struct {
	char id[16];
    netconn* conn;
} EasyQSession;

EQSession * easyq_init(const char* session_id);




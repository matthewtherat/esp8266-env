
#include "easyq.h"


err_t bind_local(EQSession* session){
	int local_port = 2048;
    err_t err;
    do {
        err = netconn_bind(session->conn, IP_ADDR_ANY, ++local_port);
    } while(err == ERR_USE && local_port < 4096);
	return err;
}


err_t easyq_init(EQSession ** session) {
    err_t err;
  
    // Wait for network connectivity
    wait_for_wifi_connection();

    // Allocating Memory
    EQSession * s = (EQSession *) malloc (sizeof (struct EQSession));
	if (s == NULL) {
		return ERR_MEM;
	}

    // Creating the connection
    s->conn = netconn_new(NETCONN_TCP);
    if (!s->conn) {
        printf("Cannot create connection");
        return ERR_MEM;
    }
    
    // Binding to local port
    err = bind_local(s);
	if (err != ERR_OK) {
		printf("Cannot bind to local address");
        return err;
	}

	// Connecting to remote host and port

    // Parsing the address
    if (!ipaddr_aton(EASYQ_HOST, s->remote_addr)) {
    	printf("Invalid IP Address: %s\n", EASYQ_HOST);
    	return ERR_VAL;
    }
    
    // Connecting
	err = netconn_connect(s->conn, s->remote_addr, EASYQ_PORT);
	if (err != ERR_OK) {
		printf("Could not connect: %s:%d\n", EASYQ_HOST, EASYQ_PORT);
		return err;
	}
	printf("Connected to: %s:%d\n", EASYQ_HOST, EASYQ_PORT);
    delay(300);
    
	// Login
	const char* greeting = "LOGIN "EASYQ_LOGIN";\n";
	err = netconn_write(s->conn, greeting, strlen(greeting), NETCONN_COPY); 
	if (err != ERR_OK) {
		printf("Could not write: %s:%d\n", EASYQ_HOST, EASYQ_PORT);
        return err;
	}
    delay(300);
    
    char * a;
    size_t len;
    easyq_read(s, &a, &len);
    s->id = (char *) malloc(len);
	if (s->id == NULL) {
		return ERR_MEM;
    }	
    strncpy(s->id, a, len);
    *session = s;

    return ERR_OK;
}


void easyq_close(EQSession ** session) {
    EQSession * s = *session;
	netconn_delete(s->conn);
	free(s);
    *session = NULL;
    s = NULL;
}


err_t easyq_read(EQSession * session, char ** line, size_t * len) {
    struct netbuf *nb;
    err_t err;

    err = netconn_recv(session->conn, &nb);
    if (err != ERR_OK) {
        return err;
    }

    err = netbuf_data(nb, (void**)line, (u16_t*)len); 
    if (err != ERR_OK) {
        return err;
    }

    return ERR_OK;
}


err_t easyq_write(EQSession * session, char * line, size_t len) {
    err_t err;
    err = netconn_write(session->conn, line, len, NETCONN_COPY); 
	if (err != ERR_OK) {
        return err;
	}
    return ERR_OK;
}


err_t easyq_push(EQSession * session, Queue * queue, char * msg, size_t len) {
    size_t size;
    if (len == -1){
        len = strlen(msg);
    }
    char line[16 + len + queue->len];
    size = sprintf(line, "PUSH %s INTO %s;\n", msg, queue->name);
    return easyq_write(session, line, size);
}


err_t easyq_pull(EQSession * session, Queue * queue) {
    size_t size;
    char line[14 + queue->len];
    size = sprintf(line, "PULL FROM %s;\n", queue->name);
    return easyq_write(session, line, size);
}


Queue * Queue_new(char * name) {
    Queue * queue = malloc (sizeof (struct Queue));
    queue->len = strlen(name);
    queue->name = name;
    return queue;
}

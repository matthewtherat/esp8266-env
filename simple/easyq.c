
#include "easyq.h"

/*
err_t bind_local(EQSession* session){
	int local_port = 2048;
    err_t err;
    do {
        err = netconn_bind(session->conn, IP_ADDR_ANY, ++local_port);
    } while(err == ERR_USE && local_port < 4096);
	return err;
}
*/

err_t easyq_init(EQSession ** s) {
    EQSession *session = malloc (sizeof (struct EQSession));
	if (session == NULL) {
		return ERR_MEM;
	}

    *s = session;
    return ERR_OK;
}


err_t easyq_connect(EQSession * s) {
    err_t err;
	const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *remote_addr;

	// Wait for wifi conection
    wait_for_wifi_connection();

    printf("Running DNS lookup for %s...\n", EASYQ_HOST);
    err = getaddrinfo(EASYQ_HOST, EASYQ_PORT, &hints, &remote_addr);

    if (err != 0 || remote_addr == NULL) {
        printf("DNS lookup failed err=%d res=%p\n", err, remote_addr);
        if(remote_addr) {
            freeaddrinfo(remote_addr);
        }
        return err;
    }

    s->socket = socket(remote_addr->ai_family, remote_addr->ai_socktype, 0);
    if(s->socket < 0) {
        printf("... Failed to allocate socket.\n");
        freeaddrinfo(remote_addr);
        return s->socket;
    }
    printf("... allocated socket\n");

    err = connect(s->socket, remote_addr->ai_addr, remote_addr->ai_addrlen);
    if(err != ERR_OK) {
        lwip_close(s->socket);
        freeaddrinfo(remote_addr);
        printf("... socket connect failed.\n");
        return err;
    }
    freeaddrinfo(remote_addr);
    printf("... connected\n");

	int timeout=400;
	err = lwip_setsockopt(s->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));
	LWIP_ASSERT("err == 0", err == ERR_OK);

    char *req = "LOGIN "EASYQ_LOGIN";\n";
    err = lwip_write(s->socket, req, strlen(req));
    if (err != ERR_OK) {
        printf("... socket send failed\n");
        lwip_close(s->socket);
        return err;
    }
    printf("... socket send success\n");

	int session_id_len = lwip_recv(s->socket, s->id, 32, 0);
	printf("Done Reading from socket: %d bytes\n", session_id_len);
	if (session_id_len < 0){
		printf("error Reading");
		lwip_close(s->socket);
        return session_id_len;
	}

    char * a;
    size_t len;
    easyq_read(s, &a, &len);
    s->id = (char *) malloc(len);
	if (s->id == NULL) {
		return ERR_MEM;
    }	
    strncpy(s->id, a, len);

    // Everithing is OK
    return ERR_OK;
}

/*
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
*/

Queue * Queue_new(char * name) {
    Queue * queue = malloc (sizeof (struct Queue));
    queue->len = strlen(name);
    queue->name = name;
    return queue;
}

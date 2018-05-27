
#include "easyq.h"


err_t easyq_connect(EQSession * s) {
    err_t err;
	const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *remote_addr;

	// Wait for wifi conection
    wait_for_wifi_connection();

    // Running DNS lookup for
    err = getaddrinfo(EASYQ_HOST, EASYQ_PORT, &hints, &remote_addr);

    if (err != 0 || remote_addr == NULL) {
        if(remote_addr) {
            freeaddrinfo(remote_addr);
        }
        return err;
    }

    s->socket = socket(remote_addr->ai_family, remote_addr->ai_socktype, 0);
    if(s->socket < 0) {
        freeaddrinfo(remote_addr);
        return s->socket;
    }
    
    // Allocated socket

    err = connect(s->socket, remote_addr->ai_addr, remote_addr->ai_addrlen);
    if(err != ERR_OK) {
        freeaddrinfo(remote_addr);
        return err;
    }
    freeaddrinfo(remote_addr);

    // Connected

    /*
	int timeout=400;
	err = lwip_setsockopt(s->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));
    if(err != ERR_OK) {
        return err;
    }
    printf("socket option is set");
    */

    // Everithing is OK
    return ERR_OK;
}


err_t easyq_login(EQSession * s) {
    err_t err;
    char * login = "LOGIN "EASYQ_LOGIN";\n";
    err = easyq_write(s, login, strlen(login));
    if (err != ERR_OK) {
        return err;
    }

    size_t len;
    char * retval;
    err = easyq_read(s, &retval, &len);
    if (err != ERR_OK) {
        return err;
    }
    s->id = malloc(len-3);
    if (s->id == NULL) {
        return ERR_MEM;
    }
    memcpy(s->id, &retval[3], len-3);
    s->id[len] = '\0';

    // Everithing is OK
    return ERR_OK;
}


void easyq_close(EQSession * s) {
	lwip_close(s->socket);
    free(s->readbuffer);
    free(s->id);
	free(s);
}


err_t easyq_init(EQSession ** session_ptr_ptr) {
    err_t err;
    EQSession * s = malloc (sizeof (struct EQSession));
	if (s == NULL) {
        easyq_close(s);
		return ERR_MEM;
	}
    s->ready = false;

    s->readbuffer = malloc(EASYQ_READ_BUFFER_SIZE);
    if (s->readbuffer == NULL) {
        easyq_close(s);
        return ERR_MEM;
    }

    err = easyq_connect(s);
    if (err != ERR_OK) {
        easyq_close(s);
        return err;
    }

    err = easyq_login(s);
    if (err != ERR_OK) {
        easyq_close(s);
        return err;
    }

    *session_ptr_ptr = s;
    s->ready = true;
    return ERR_OK;
}


err_t easyq_read(EQSession * s, char ** line, size_t * len) {
    err_t err_or_len;
    s->readbuffer[0] = '\0';
    char buff[1];
    size_t bytes = 0;
    while(bytes < EASYQ_READ_BUFFER_SIZE) {
        err_or_len = lwip_recv(s->socket, buff, 1, 0);
        if (err_or_len < 0){
        	return err_or_len;
        } 
        
        if (err_or_len == 0 || (bytes == 0 && buff[0] == '\n')) {
            continue;
        }

        if (buff[0] == ';') {
            s->readbuffer[bytes] = '\0';
            *line = s->readbuffer;
            *len = bytes+1;
            return ERR_OK;
        }
        s->readbuffer[bytes] = buff[0];
        bytes++;
    } 
    return ERR_MEM;
}


err_t easyq_write(EQSession * s, char * line, size_t len) {
    err_t err_or_len = lwip_write(s->socket, line, len);
    if (err_or_len < ERR_OK) {
        return err_or_len;
    }
    return ERR_OK;
}


err_t easyq_push(EQSession * s, Queue * queue, char * msg, size_t len) {
    size_t size;
    if (len == -1){
        len = strlen(msg);
    }
    char line[16 + len + queue->len];
    size = sprintf(line, "PUSH %s INTO %s;\n", msg, queue->name);
    return easyq_write(s, line, size);
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

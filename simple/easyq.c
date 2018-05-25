

EQSession * easyq_init(const char* session_id) {
    ip_addr_t local_addr;
    u16_t local_port;
    err_t err;
  
    // Allocating Memory
	EQSession *session = malloc (sizeof (struct EQSession));
	if (session == NULL) {
		return NULL;
	}
    
    // Creating the connection
    session->conn = netconn_new(NETCONN_TCP);
    if (!conn) {
        printf("Cannot create connection");
    }

    // Binding to local port

	return session;
}


void easyq_close(EQSession * session) {
	free(session);
}


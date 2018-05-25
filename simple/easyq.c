
EQSession * easyq_init(const char* session_id) {
	EQSession *session = malloc (sizeof (struct EQSession));
	if (session == NULL) {
		return NULL;
	}

	return session;
}


void easyq_close(EQSession * session) {
	free(session);
}


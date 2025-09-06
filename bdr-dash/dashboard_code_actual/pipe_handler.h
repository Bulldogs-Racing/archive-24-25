#ifndef PIPE_HANDLER_H
#define PIPE_HANDLER_H

enum MessageTypes {
	DEBUG = 0,
	HEARTBEAT = 1,
    CANDRIVEORNOT = 2
};

int setup_dashboard();

int send_dashboard(enum MessageTypes m, char * message);

#endif


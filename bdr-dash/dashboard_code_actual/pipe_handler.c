#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "pipe_handler.h"

FILE * fp;

// spawn python and ui, setup pipe
int setup_dashboard(){
	fp = popen("python /home/bdr/dashboard_code_actual/tkinter_dashboard.py", "w");
	if(fp == NULL){
		printf("ERROR! %s\n", strerror(errno));
		return 1;
	}
	return 0;
}

int send_dashboard(enum MessageTypes m, char * message){
	//printf("DEBUG: %d:%s\n", m, message);
	int r = fprintf(fp, "%d:%s\n", m, message);
        fflush(fp);
	if(r < 0){
		printf("ERROR! %s\n", strerror(errno));
		return 1;
	}
	return 0;
}


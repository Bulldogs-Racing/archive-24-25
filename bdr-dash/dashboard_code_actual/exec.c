#include <stdio.h>
#include <stdlib.h>

#include "can_send.h"

#define DEFAULT_CAN_ID 0x7FE

int main(int argc, char * argv[]){
    int can_id_to_send = DEFAULT_CAN_ID;
    if(argc >= 2){
        int rv = atol(argv[1]);
        if(rv != 0 && rv != 0x7FF){
            printf("setting canid to send to %d\n", rv);
            can_id_to_send = rv;
        }
    }
	printf("hello world!");
	can_send_message(can_id_to_send);
	printf("sent!");
	return 0;
}

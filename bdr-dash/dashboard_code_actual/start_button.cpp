#include <stdio.h>
#include <stdlib.h>
#include <lgpio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUZZER_GPIO 17
#define BUTTON_GPIO 4
#define HV_GPIO 22

#define CAR_START_BUTTON_ID 0x7FE
#define CAR_HV_OFF_ID 0x7FD

#define DASHBOARD_CAR_ON_SIGNAL (SIGUSR1)
#define DASHBOARD_CAR_OFF_SIGNAL (SIGUSR2)

// SENDS 0x7FE on start button press
// SENDS 0x7FD on hv turning off when start button pressed

char started = 0;
char launched = 0;

void activate_buzzer(int h){
	lgGpioWrite(h, BUZZER_GPIO, 1);
	sleep(3);
	lgGpioWrite(h, BUZZER_GPIO, 0);
}

// updates dashboard on if car is on (actually somewhat important like yn we sort of want this :3)
void signal_send_handler(int pid, int sigtype){
	if(pid == -1){
		return;
	}
    kill(pid, sigtype);
}

int fork_handler(){
	int fs = fork();
	printf("FORK FORK FORK\n");
	fflush(stdout);
	if(fs == -1){
		perror("fork fail");
	} else if (fs == 0){
		// child
		char *args2[] = {"/home/bdr/dashboard_code_actual/receiver", NULL};
		execvp(args2[0], args2);
        return 0;
	} else {
		printf("FORKED, CHILD PID %d\n", fs);
        return fs;
	}
	return 0;
}

void fork_handler_send_can(int can_id){
	int fs = fork();
	if(fs == -1){
		perror("fork fail");
	} else if (fs == 0){
		// child
        char *id_buff = (char *)calloc(1, 16);
        snprintf(id_buff, 15, "%d", can_id);
		char *args[] = {"/home/bdr/dashboard_code_actual/exec", id_buff, NULL};
		printf("running execvp\n");
		int status_code = execvp(args[0], args);
		if(status_code == -1){
			printf("ERRORED! %s\n", strerror(errno));
		}
        free(id_buff);
	} else {
		printf("FORKED, CHILD PID %d\n", fs);
	}
	return;
}

char get_button_status(int h){
	//return 1;
	return (char)(lgGpioRead(h, BUTTON_GPIO));
}

char get_hv_status(int h){
	//return 1;
	return (char)(lgGpioRead(h, HV_GPIO));
}

// returns gpio handle
int gpio_setup(){
	int h = lgGpiochipOpen(0);
	if(h < 0){
		perror("lgpio open fail");
		return 1;
	}
	// gpio 17 (aka. 11)
	if(lgGpioClaimOutput(h, 0, BUZZER_GPIO, 0)){
		perror("gpio claim");
		return 1;
	}
	// gpio 27 (aka 13)
	if(lgGpioClaimInput(h, LG_SET_PULL_DOWN, BUTTON_GPIO)){
		perror("gpio input setup");
		return 1;
	}
	if(lgGpioClaimInput(h, LG_SET_PULL_DOWN, HV_GPIO)){
		perror("hv input setup");
		return 1;
	}
	return h;
}

int main(){
	freopen("/home/bdr/dashboard_code_actual/start_button_log.txt", "a+", stdout);
	int handler = gpio_setup();
    	int receiver_pid = -1; // dont immediately start dashboard


	// TEMPORARY CODE FOR DEBUGGING TEENSY DUE TO WATCHDOG INCOMPATIBILITY
	// REMOVE BEFORE CAR IS TESTED
	// UNSAFE
	//fork_handler_send_can(CAR_START_BUTTON_ID);
	// IMPORTANT! COMMENT OUT THE LINE ABOVE THIS ^ !!! IMPORTANT !


	//sleep(10);
	printf("HI\n");
	fflush(stdout);
	started = 0;
	while(1){
		char on = get_button_status(handler);
	        char hv_on = get_hv_status(handler);
		printf("on: %c\n", on + '0');
		printf("hv on: %c\n", hv_on + '0');
		if(!launched && (on)){
			// launch dashboard on first start button press
			launched = 1;
			receiver_pid = fork_handler();
		}
		if(!started && on && hv_on){
        	    // start car start sequence
	            activate_buzzer(handler);
            		if(hv_on){
        	        // actually turn on
               		 fork_handler_send_can(CAR_START_BUTTON_ID);
	              	  started = 1;
        	        signal_send_handler(receiver_pid, DASHBOARD_CAR_ON_SIGNAL);
	            }
				continue;
			}
	        if(started && !hv_on){
        	    // HV OFF TURN OFF
	            fork_handler_send_can(CAR_HV_OFF_ID);
            	started = 0;
        	    signal_send_handler(receiver_pid, DASHBOARD_CAR_OFF_SIGNAL);
	        }
		usleep(100000);
	}
	//fork_handler();
	//activate_buzzer(handler);
	//fork_handler_send_can(0x7FE);
	// todo: logic! here!
	return 0;
}

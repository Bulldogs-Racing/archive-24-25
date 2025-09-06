#include <stdio.h>
#include <stdlib.h>
#include <lgpio.h>
#include <unistd.h>

#define BUZZER_GPIO 17

void activate_buzzer(int h){
	lgGpioWrite(h, BUZZER_GPIO, 1);
	sleep(3);
	lgGpioWrite(h, BUZZER_GPIO, 0);
}

// returns gpio handle
int buzzer_setup(){
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
	return h;
}

int main(){
	int handler = buzzer_setup();
	activate_buzzer(handler);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <lgpio.h>
#include <unistd.h>

#define BUZZER_GPIO 17
#define HV_GPIO 22

void activate_buzzer(int h){
	lgGpioWrite(h, BUZZER_GPIO, 1);
	sleep(3);
	lgGpioWrite(h, BUZZER_GPIO, 0);
}

char get_button_status(int h){
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
	// gpio 22 (aka idk)
	if(lgGpioClaimInput(h, LG_SET_PULL_DOWN, HV_GPIO)){
		perror("gpio input setup");
		return 1;
	}
	return h;
}

int main(){
	int handler = gpio_setup();
	while(1){
		char on = get_button_status(handler);
		printf("on: %c\n", on + '0');
		sleep(1);
	}
	activate_buzzer(handler);
	return 0;
}

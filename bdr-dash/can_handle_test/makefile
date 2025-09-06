all: lcd buzzer_test button hv_test

button: button_test.cpp
	g++ button_test.cpp -Wall -o button -llgpio

hv_test: hv_test.cpp
	g++ hv_test.cpp -Wall -o hv_test -llgpio

lcd: display_can_to_lcd.cpp
	g++ display_can_to_lcd.cpp -Wall -o lcd -llgpio

buzzer_test: buzzer_handler.cpp
	g++ buzzer_handler.cpp -Wall -o buzzer_test -llgpio


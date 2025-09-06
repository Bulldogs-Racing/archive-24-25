
//CAN BUS should communicate with SPI with libcanard
// compile with g++ display_can_to_lcd.cpp -o lcd -llgpio

//if no socket.h:
//sudo apt install libc6-dev

// TO DO: RIGHT NOW THIS SCRIPT CLEARS THE SCREEN TO DISPLAY THE CAN ERROR, USE TO TEST, THEN CONNECT WITH TK SCRIPT

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <lgpio.h>
#include <unistd.h>
#include <string.h>
#include <linux/can.h>
#include <linux/can/raw.h> //uses SocketCAN framework: https://github.com/linux-can/can-utils 
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>


//TO DO: DECIDE WHAT EACH OF THE CONSTANTS ARE BASED OFF PINS AND SPEED WE USE IN ACTUAL SET UP

// Define constants for SPI interface and CAN bus       
#define SPI_CHANNEL 0 // SPI channel (0 or 1 depending on which connects Pi to HAT)
#define SPI_SPEED 500000 // SPI communication speed in Hz (find max SPI speed supported by CAN HAT)
#define CAN_HAT_CS_PIN 8 // Chip Select Pin for CAN HAT, (find wiring config on CAN HAT specs)
#define LCD_RS 3   // LCD register select pin (which reg connected to CAN, command: 0, data: 1)
#define LCD_E  2   // LCD enable pin (allows writing to LCD regs if high)
#define LCD_D4 14  // LCD data pin 4
#define LCD_D5 13  // LCD data pin 5
#define LCD_D6 12  // LCD data pin 6
#define LCD_D7 11  // LCD data pin 7
#define GPIOD_OUT_LOW 0

int chip; //lgGpiochip

// Function headers
void initLCD();
void lcd_send_command(unsigned char cmd);
void lcd_send_data(unsigned char data);
void lcd_print(const char *str);
void log_CAN_error();
void lcd_clear();


// Initialize the LCD screen (4-bit mode)
void initLCD() {
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_RS, 0);
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_E, 0);
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_D4, 0);
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_D5, 0);
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_D6, 0);
    lgGpioClaimOutput(chip, GPIOD_OUT_LOW, LCD_D7, 0);
    
    // TO DO: SEND COMMAND TO TK SCRIPT?

    lcd_send_command(0x33); // Initialization sequence for 4-bit mode
    lcd_send_command(0x32); // Set to 4-bit mode
    lcd_send_command(0x28); // 2 lines, 5x7 matrix
    lcd_send_command(0x0C); // Display on, cursor off
    lcd_send_command(0x06); // Increment cursor
    lcd_clear();            // Clear display
}

// Send command to the LCD
void lcd_send_command(unsigned char cmd) {
    // Selects the LCD's Instruction Register (which tells LCD what to do next)
    lgGpioWrite(chip, LCD_RS, 0); 
    lgGpioWrite(chip, LCD_E, 1);

    // Send upper nibble
    lgGpioWrite(chip, LCD_D4, (cmd >> 4) & 0x01);
    lgGpioWrite(chip, LCD_D5, (cmd >> 4) & 0x02);
    lgGpioWrite(chip, LCD_D6, (cmd >> 4) & 0x04);
    lgGpioWrite(chip, LCD_D7, (cmd >> 4) & 0x08);
    lgGpioWrite(chip, LCD_E, 0);
    usleep(100);
    lgGpioWrite(chip, LCD_E, 1);

    // Send lower nibble
    lgGpioWrite(chip, LCD_D4, cmd & 0x01);
    lgGpioWrite(chip, LCD_D5, cmd & 0x02);
    lgGpioWrite(chip, LCD_D6, cmd & 0x04);
    lgGpioWrite(chip, LCD_D7, cmd & 0x08);
    lgGpioWrite(chip, LCD_E, 0);
    usleep(100);
}

// Send data to the LCD
void lcd_send_data(unsigned char data) {
    // Selects the LCD's Data Register (hold info on screen)
    lgGpioWrite(chip, LCD_RS, 1); 
    lgGpioWrite(chip, LCD_E, 1);

    // Send upper nibble
    lgGpioWrite(chip, LCD_D4, (data >> 4) & 0x01);
    lgGpioWrite(chip, LCD_D5, (data >> 4) & 0x02);
    lgGpioWrite(chip, LCD_D6, (data >> 4) & 0x04);
    lgGpioWrite(chip, LCD_D7, (data >> 4) & 0x08);
    lgGpioWrite(chip, LCD_E, 0);
    usleep(100);
    lgGpioWrite(chip, LCD_E, 1);

    // Send lower nibble
    lgGpioWrite(chip, LCD_D4, data & 0x01);
    lgGpioWrite(chip, LCD_D5, data & 0x02);
    lgGpioWrite(chip, LCD_D6, data & 0x04);
    lgGpioWrite(chip, LCD_D7, data & 0x08);
    lgGpioWrite(chip, LCD_E, 0);
    usleep(100);
}

// Print string to LCD
// TO DO: SEND DATA TO TK SCRIPT?
void lcd_print(const char *str) { 
    while (*str) {
        lcd_send_data(*str++);
    }
}

// Clear the LCD display
void lcd_clear() {
    lcd_send_command(0x01); // Clear display command
    usleep(2000);           // Delay for command to take effect
}

// Log CAN bus error message to the LCD
void log_CAN_error() {
    struct can_frame frame;
    struct sockaddr_can addr;
    struct ifreq ifr;

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket creation failed");
        return;
    }

    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Binding socket failed");
        close(s);
        return;
    }

    // Blocking read from CAN bus
    if (read(s, &frame, sizeof(struct can_frame)) < 0) {
        perror("CAN read error");
    } else {
        // Display error message on LCD
        lcd_clear();
        lcd_print("CAN Error:");
        char errorMsg[16];
        snprintf(errorMsg, 16, "ID: 0x%X", frame.can_id);
        lcd_print(errorMsg);
        snprintf(errorMsg, 16, "Data: %02X %02X", frame.data[0], frame.data[1]);
        lcd_print(errorMsg);
    }

    close(s);
}


int main(void) {

    // Initialize wiringPi
    chip = lgGpiochipOpen(0);
    if (chip < 0) {
        perror("lgGpio Open");
        return 1;
    }

    // Initialize SPI communication
    if (lgSpiOpen(chip, SPI_CHANNEL, SPI_SPEED, 0) < 0) {
        perror("SPI setup");
        return 1;
    }

    // Initialize the LCD display
    initLCD();    
    
    while (1) {
        log_CAN_error(); // Log CAN error message to LCD
        sleep(5); 
    }

    lgGpiochipClose(chip);
    return 0;
}


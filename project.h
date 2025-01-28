// project.h - Header File
#ifndef PROJECT_H
#define PROJECT_H

#include <lpc17xx.h>
#include <stdio.h>
#include <string.h>

// Define pins and ports
#define BUZZER_PIN (1 << 0) // P0.0 for Buzzer
#define LED_PIN    (1 << 1) // P0.1 for LED
#define LIGHT_PIN  (1 << 2) // P0.2 for Light Control
#define FAN_PIN    (1 << 3) // P0.3 for Fan Control

// Function declarations
void lcd_init(void);
void lcd_clear(void);
void lcd_write(char *data);
void keypad_init(void);
char keypad_getkey(void);
void delay_ms(unsigned int ms);
void mark_attendance(void);
void quiz_test(void);
void display_info(void);
void toggle_light_fan(void);

#endif

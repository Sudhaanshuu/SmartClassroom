#ifndef SMART_CLASSROOM_H
#define SMART_CLASSROOM_H

#include <LPC17xx.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Pin Definitions
#define ROW (0x0F << 4)  // port2.7 to 2.4
#define COL (0x0F)       // port2.3 to port2.0
#define RS (1<<10)       // Register Select for LCD
#define EN (1<<11)       // Enable for LCD
#define VP (0xFF <<15)    // LCD data pins P0.15-P0.22
#define BUZZ (1<<27)     // Buzzer pin
#define LED (1<<26)      // LED pin
#define FAN (1<<20)      // Fan control pin
#define LIGHT (1<<21)    // Light control pin
#define ALL_LED (0xFF << 19)

// Constants
#define MAX_STUDENTS 10
#define REG_LENGTH 3    // 10 digits + null terminator
#define MAX_QUESTIONS 5

// Structures
typedef struct {
    char name[20];
    char reg_no[REG_LENGTH];
    uint8_t present;
} Student;

typedef struct {
    char question[32];
    char options[4][8];
    char correct_option;
} Question;

// Function Prototypes
// LCD Functions
void lcd_init(void);
void lcd_cmd_write(char c);
void lcd_data_write(char d);
void lcd_str_write(char *pstr);
void lcd_clear(void);

// Keypad Functions
void keypad_init(void);
char get_key(void);
void get_string(char *str, uint8_t max_len);

// Menu Functions
void show_menu(void);
void handle_attendance(void);
void handle_quiz(void);
void handle_display(void);
void handle_controls(void);
void handle_emy(void);
void handle_exit(void);

// Utility Functions
void delay_ms(uint32_t ms);
void buzz_error(void);
float read_temperature(void);
void initialize_students(void);
char* get_random_subject(void);
char* get_random_time(void);
// External Variables
extern Student students[MAX_STUDENTS];
extern uint8_t class_strength;
extern Question quiz_questions[MAX_QUESTIONS];
extern uint8_t current_temperature;
extern uint8_t fan_status;
extern uint8_t light_status;

#endif // SMART_CLASSROOM_H


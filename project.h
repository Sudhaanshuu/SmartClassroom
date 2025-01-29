/* smartclass.h */
#include <LPC17xx.h>
#include <string.h>
#include <stdlib.h>

#define ROW (0x0F << 4)
#define COL (0x0F)
#define BUZZ (1<<27)
#define LED (1<<28)
#define LIGHT (1<<29)
#define FAN (1<<30)

typedef struct {
    char reg[11];
    char name[20];
    int present;
} Student;

extern Student students[10];
extern uint8_t current_strength;
extern char* subjects[];
extern uint8_t menu_option;

void lcd_init();
void lcd_clear();
void lcd_str_write(char* str);
void lcd_cmd_write(char cmd);
void lcd_data_write(char data);
void delay(uint32_t ms);
char keypad_read();
void buzzer_alert();
void toggle_device(uint32_t device);
void f1_attendance();
void f2_quiz();
void f3_display_info();
void f4_control_devices();
void show_menu();
float read_temperature();
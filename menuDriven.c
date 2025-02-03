
// main.c
#include "project.h"

// Global variables
Student students[MAX_STUDENTS];
uint8_t class_strength = 0;
uint8_t current_temperature = 0;
uint8_t fan_status = 0;
uint8_t light_status = 0;
uint8_t auto_fan_mode = 0;
uint8_t auto_light_mode = 0;
uint16_t ldr_value = 0;

Question quiz_questions[MAX_QUESTIONS] = {
    {"2+4=?", {"4", "6", "8", "9"}, 'B'},
    {"7-3=?", {"2", "3", "4", "5"}, 'C'},
    {"5x2=?", {"8", "10","12","6"}, 'B'},
    {"9/3=?", {"2", "3", "4", "5"}, 'B'},
    {"1+1=?", {"1", "2", "3", "4"}, 'B'}
};

int main(void) {
    char key;
    
    // Initialize hardware
    lcd_init();
    keypad_init();
    initialize_controls();
    
    // Initialize student data
    initialize_students();
    
    while(1) {
        show_menu();
        key = get_key();
        
        switch(key) {
            case '1': // FA - Attendance
                handle_attendance();
                break;
                
            case '2': // FQ - Quiz
                handle_quiz();
                break;
                
            case '3': // FD - Display
                handle_display();
                break;
                
            case '4': // FC - Controls
                handle_controls();
                break;
                
            case '5': // FE - Emergency 
                handle_emy();
                break;
                
            case '6': // FX - Exit
                handle_exit();
                break;
                
            default:
                lcd_clear();
                lcd_str_write("Invalid Choice!");
                delay_ms(1000);
        }
    }
}

						

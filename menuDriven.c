// main.c
#include "project.h"

// Global variables
Student students[MAX_STUDENTS];
uint8_t class_strength = 0;
uint8_t current_temperature = 0;
uint8_t fan_status = 0;
uint8_t light_status = 0;

Question quiz_questions[MAX_QUESTIONS] = {
    {"2+4=?", {"4", "6", "8", "9"}, '2'},
    {"7-3=?", {"2", "3", "4", "5"}, '3'},
    {"5x2=?", {"8", "10","12","6"}, '2'},
    {"9/3=?", {"2", "3", "4", "5"}, '2'},
    {"1+1=?", {"1", "2", "3", "4"}, '2'}
};

int main(void) {
    char key;
    
    // Initialize hardware
    lcd_init();
    keypad_init();
    
    // Initialize GPIO for controls
    LPC_GPIO1->FIODIR |= BUZZ | LED;
    LPC_GPIO1->FIODIR |= FAN | LIGHT;
    
    // Initialize ADC for temperature
    LPC_SC->PCONP |= (1<<12);
    LPC_PINCON->PINSEL1 |= (1<<18);
    LPC_PINCON->PINSEL1 &= ~(1<<19);
    LPC_ADC->ADCR |= (1<<2) | (1<<21);
    
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
                
            default:
                lcd_clear();
                lcd_str_write("Invalid Choice!");
                delay_ms(1000);
        }
    }
}



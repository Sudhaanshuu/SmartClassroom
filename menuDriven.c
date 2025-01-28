// main.c - Main Code
#include "project.h"

void menu(void) {
    lcd_clear();
    lcd_write("1. F1 Attendance\n2. F2 Quiz Test\n3. F3 Info\n4. F4 Control\n");
    delay_ms(2000);
}

int main(void) {
    char choice;

    // Initialize peripherals
    lcd_init();
    keypad_init();
    LPC_GPIO0->FIODIR |= BUZZER_PIN | LED_PIN | LIGHT_PIN | FAN_PIN; // Set control pins as output

    while (1) {
        menu();
        choice = keypad_getkey();

        switch (choice) {
            case '1':
                mark_attendance();
                break;
            case '2':
                quiz_test();
                break;
            case '3':
                display_info();
                break;
            case '4':
                toggle_light_fan();
                break;
            default:
                lcd_clear();
                lcd_write("Invalid Choice\n");
                delay_ms(1000);
                break;
        }
    }

 
}

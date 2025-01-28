// features.c - Feature Implementations
#include "project.h"

void mark_attendance(void) {
    char reg[11];
    int i;

    lcd_clear();
    lcd_write("Enter Reg No:\n");

    for (i = 0; i < 10; i++) {
        reg[i] = keypad_getkey();
        lcd_write(&reg[i]); // Display entered digit
        delay_ms(300);
    }
    reg[10] = '\0';

    if (strcmp(reg, "1234567890") == 0) { // Example valid reg number
        lcd_clear();
        lcd_write("Attendance Marked\n");
        delay_ms(1000);
    } else {
        lcd_clear();
        lcd_write("Invalid Reg\n");
        LPC_GPIO0->FIOSET = BUZZER_PIN | LED_PIN; // Turn on buzzer and LED
        delay_ms(1000);
        LPC_GPIO0->FIOCLR = BUZZER_PIN | LED_PIN; // Turn off buzzer and LED
    }
}

void quiz_test(void) {
    char answer;

    // Question 1
    lcd_clear();
    lcd_write("1. 2+2=?\nA.3 B.4\nC.5 D.6\n");
    answer = keypad_getkey();

    if (answer == 'B') {
        lcd_clear();
        lcd_write("Correct!\n");
        delay_ms(1000);
    } else {
        lcd_clear();
        lcd_write("Wrong!\n");
        LPC_GPIO0->FIOSET = BUZZER_PIN | LED_PIN;
        delay_ms(1000);
        LPC_GPIO0->FIOCLR = BUZZER_PIN | LED_PIN;
    }

    // Add more questions similarly
}

void display_info(void) {
    lcd_clear();
    lcd_write("Temp: 25C\nNext: Math\n");
    delay_ms(2000);
}

void toggle_light_fan(void) {
    static int light_state = 0;
    static int fan_state = 0;

    light_state = !light_state;
    fan_state = !fan_state;

    if (light_state) {
        LPC_GPIO0->FIOSET = LIGHT_PIN;
    } else {
        LPC_GPIO0->FIOCLR = LIGHT_PIN;
    }

    if (fan_state) {
        LPC_GPIO0->FIOSET = FAN_PIN;
    } else {
        LPC_GPIO0->FIOCLR = FAN_PIN;
    }

    lcd_clear();
    lcd_write("Light & Fan\nToggled\n");
    delay_ms(1000);
}

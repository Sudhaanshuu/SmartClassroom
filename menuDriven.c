/* smartclass.c (main) */
#include "project.h"

int main() {
 	char input;
    // Initialize hardware
    LPC_GPIO1->FIODIR |= BUZZ|LED|LIGHT|FAN;
    LPC_GPIO2->FIODIR |= ROW;
    LPC_GPIO2->FIODIR &= ~COL;
  
    lcd_init();
    srand(LPC_RTC->SEC);
    
    
    while(1) {
        show_menu();
         input = keypad_read();
        
        switch(input) {
            case '1': f1_attendance(); break;
            case '2': f2_quiz(); break;
            case '3': f3_display_info(); break;
            case '4': f4_control_devices(); break;
        }
    }
}

// Implement remaining helper functions
void toggle_device(uint32_t device) {
    LPC_GPIO1->FIOPIN ^= device;
}

void delay(uint32_t ms) {
    uint32_t i,j;
    for( i=0; i<ms; i++)
        for( j=0; j<1250; j++);
}

void lcd_cmd_write(char cmd) {
    LPC_GPIO0->FIOCLR = 0xFF<<15;
    LPC_GPIO0->FIOSET = cmd<<15;
    LPC_GPIO0->FIOCLR = (1<<27); // RS=0
    LPC_GPIO0->FIOSET = (1<<28); // EN=1
    delay(2);
    LPC_GPIO0->FIOCLR = (1<<28);
    delay(2);
}

void lcd_data_write(char data) {
    LPC_GPIO0->FIOCLR = 0xFF<<15;
    LPC_GPIO0->FIOSET = data<<15;
    LPC_GPIO0->FIOSET = (1<<27); // RS=1
    LPC_GPIO0->FIOSET = (1<<28); // EN=1
    delay(2);
    LPC_GPIO0->FIOCLR = (1<<28);
    delay(2);
}
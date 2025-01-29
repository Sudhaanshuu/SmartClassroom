/* smartclass_functions.c */
#include "project.h"

Student students[10] = {
    {"1000000001", "Rahul Sharma", 0},
    {"1000000002", "Priya Patel", 0},
    // Add remaining 8 students with Indian names
    // ...
};

uint8_t current_strength = 0;
char* subjects[] = {"Math", "Science", "History", "English", "CS"};
uint8_t menu_option = 0;

void lcd_init() {
    LPC_GPIO0->FIODIR |= (0xFF<<15) | (1<<27) | (1<<28);
    lcd_cmd_write(0x38);
    lcd_cmd_write(0x0E);
    lcd_cmd_write(0x01);
    lcd_cmd_write(0x06);
}

void lcd_clear() { lcd_cmd_write(0x01); }

void lcd_str_write(char* str) {
    while(*str) lcd_data_write(*str++);
}

void buzzer_alert() {
    LPC_GPIO1->FIOSET = BUZZ;
    LPC_GPIO1->FIOSET = LED;
    delay(500);
    LPC_GPIO1->FIOCLR = BUZZ|LED;
}

char keypad_read() {
    uint32_t code[] = {0x0E, 0x0D, 0x0B, 0x07};
    char ktab[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for(int i=0; i<4; i++) {
        LPC_GPIO2->FIOCLR = ROW;
        LPC_GPIO2->FIOSET = code[i] << 4;
        uint32_t col_val = LPC_GPIO2->FIOPIN & COL;
        
        for(int j=0; j<4; j++) 
            if(col_val == code[j]) 
                return ktab[i][j];
    }
    return 0;
}

void f1_attendance() {
    char input[11] = {0};
    lcd_clear();
    lcd_str_write("Enter Reg No:");
    lcd_cmd_write(0xC0);

    for(int i=0; i<10; i++) {
        while(!(input[i] = keypad_read()));
        lcd_data_write(input[i]);
        delay(200);
    }

    for(int i=0; i<10; i++) {
        if(strcmp(students[i].reg, input) == 0) {
            students[i].present++;
            current_strength++;
            lcd_clear();
            lcd_str_write(students[i].name);
            lcd_cmd_write(0xC0);
            lcd_str_write("Present!");
            delay(2000);
            return;
        }
    }
    buzzer_alert();
    lcd_clear();
    lcd_str_write("Invalid Reg No!");
    delay(2000);
}

void f2_quiz() {
    struct { char* q; char a; } questions[] = {
        {"2+4=?", '6'},
        {"5-3=?", '2'},
        {"3*3=?", '9'},
        // Add more questions
    };

    for(int i=0; i<3; i++) {
        lcd_clear();
        lcd_str_write(questions[i].q);
        lcd_cmd_write(0xC0);
        lcd_str_write("A B C D");

        char ans = keypad_read();
        if(ans != questions[i].a) {
            buzzer_alert();
            lcd_clear();
            lcd_str_write("Wrong!");
            delay(1000);
        }
    }
}

void f3_display_info() {
    lcd_clear();
    char info[16];
    sprintf(info, "Temp:%.1fC", read_temperature());
    lcd_str_write(info);
    delay(2000);
    
    lcd_clear();
    sprintf(info, "Next: %s", subjects[rand()%5]);
    lcd_str_write(info);
    delay(2000);
    
    lcd_clear();
    sprintf(info, "Strength: %d", current_strength);
    lcd_str_write(info);
    delay(2000);
}

void f4_control_devices() {
    static uint8_t light_state = 0, fan_state = 0;
    lcd_clear();
    lcd_str_write("A:Light B:Fan");
    
    while(1) {
        char key = keypad_read();
        if(key == 'A') toggle_device(LIGHT);
        if(key == 'B') toggle_device(FAN);
        if(key == '#') return;
    }
}

float read_temperature() {
    LPC_ADC->ADCR |= (1<<24);
    while(!(LPC_ADC->ADDR2 & (1<<31)));
    uint32_t adc_value = (LPC_ADC->ADDR2 >> 4) & 0xFFF;
    return (adc_value * 3.3 / 4096) * 100;
}

void show_menu() {
    lcd_clear();
    lcd_str_write("1:Att 2:Quiz");
    lcd_cmd_write(0xC0);
    lcd_str_write("3:Info 4:Ctrl");
}
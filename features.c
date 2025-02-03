
// functions.c
#include "project.h"

// Keypad configuration
static uint32_t code[] = {0x0E, 0x0D, 0x0B, 0x07};
static char ktab[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

// Subject and time lists
static char* time[] = {"7-8AM","8-9AM","2-3PM","3-4PM","4-5PM"};
static char* subjects[] = {"PYTHON", "A.I", "ARM", "C LANGUAGE", "JAVA"};

void initialize_controls(void) {
    // Initialize GPIO for controls
    LPC_GPIO1->FIODIR |= BUZZ | MOTOR | ALL_LED;
    LPC_GPIO1->FIODIR |= FAN | LIGHT;
    LPC_GPIO1->FIOCLR = ALL_LED;
    
    // Initialize ADC for temperature and LDR
    LPC_SC->PCONP |= (1<<12);
    
    // Temperature sensor on AD0.2
    LPC_PINCON->PINSEL1 |= (1<<18);
    LPC_PINCON->PINSEL1 &= ~(1<<19);
    
    // LDR on AD0.3
    LPC_PINCON->PINSEL1 |= (1<<20);
    LPC_PINCON->PINSEL1 &= ~(1<<21);
    
    LPC_ADC->ADCR |= (1<<2) | (1<<3) | (1<<21); // Enable both channels
}

void lcd_init(void) {
    LPC_GPIO0->FIODIR |= RS | VP | EN;
    
    lcd_cmd_write(0x38); // 8 bit mode 2 line 
    lcd_cmd_write(0x0E); // cursor on display on
    lcd_cmd_write(0x01); // clear screen
    lcd_cmd_write(0x0C);
}

void handle_controls(void) {
    char key;
	char auto_key;
	static uint32_t last_display;
	uint8_t refresh_display;
	static uint32_t last_sensor_read;
     refresh_display = 1;
    
    while(1) {
        // Only update display when needed
        if(refresh_display) {
            lcd_clear();
            lcd_str_write("1:Fan 2:Light");
            lcd_cmd_write(0xC0);
            lcd_str_write("3:Auto *:Back");
            refresh_display = 0;
        }

        // Read sensors at fixed intervals
         last_sensor_read = 0;
        if(last_sensor_read++ > 10) {
            current_temperature = read_temperature();
            ldr_value = read_ldr();
            last_sensor_read = 0;
            
            // Auto control with hysteresis
            if(auto_fan_mode) {
                if(current_temperature >= TEMP_THRESHOLD + 2) {
                    fan_status = 1;
                    LPC_GPIO1->FIOSET = FAN | MOTOR;
                } else if(current_temperature <= TEMP_THRESHOLD - 2) {
                    fan_status = 0;
                    LPC_GPIO1->FIOCLR = FAN | MOTOR;
                }
            }
            
            if(auto_light_mode) {
                if(ldr_value < LIGHT_THRESHOLD - 100) {
                    light_status = 1;
                    LPC_GPIO1->FIOSET = LIGHT;
                } else if(ldr_value > LIGHT_THRESHOLD + 100) {
                    light_status = 0;
                    LPC_GPIO1->FIOCLR = LIGHT;
                }
            }
        }
        
        // Check for user input
        key = get_key();
        switch(key) {
            case '1':
                if(!auto_fan_mode) {
                    fan_status = !fan_status;
                    if(fan_status) {
                        LPC_GPIO1->FIOSET = FAN | MOTOR;
                    } else {
                        LPC_GPIO1->FIOCLR = FAN | MOTOR;
                    }
                    refresh_display = 1;
                }
                break;
                
            case '2':
                if(!auto_light_mode) {
                    light_status = !light_status;
                    if(light_status) {
                        LPC_GPIO1->FIOSET = LIGHT;
                    } else {
                        LPC_GPIO1->FIOCLR = LIGHT;
                    }
                    refresh_display = 1;
                }
                break;
                
            case '3':
                lcd_clear();
                lcd_str_write("Auto: 1:Fan");
                lcd_cmd_write(0xC0);
                lcd_str_write("2:Light *:Back");
                
                 auto_key = get_key();
                if(auto_key == '1') {
                    auto_fan_mode = !auto_fan_mode;
                    if(!auto_fan_mode) {
                        fan_status = 0;
                        LPC_GPIO1->FIOCLR = FAN | MOTOR;
                    }
                } else if(auto_key == '2') {
                    auto_light_mode = !auto_light_mode;
                    if(!auto_light_mode) {
                        light_status = 0;
                        LPC_GPIO1->FIOCLR = LIGHT;
                    }
                }
                refresh_display = 1;
                break;
                
            case '*':
                return;
        }
        
        // Status display with fixed interval updates
         last_display = 0;
        if(last_display++ > 5) {
            lcd_clear();
            lcd_str_write(fan_status ? "Fan:ON " : "Fan:OFF ");
            lcd_str_write(light_status ? "L:ON" : "L:OFF");
            lcd_cmd_write(0xC0);
            if(auto_fan_mode) lcd_str_write("AutoF ");
            if(auto_light_mode) lcd_str_write("AutoL");
            last_display = 0;
        }
        
        delay_ms(50);
    }
}

void show_menu(void) {
    lcd_clear();
    lcd_str_write("1:AD 2:QZ 3:DIS");
    lcd_cmd_write(0xC0);
    lcd_str_write("4:CT 5:EM 6:END");
}

void handle_attendance(void) {
    char reg_no[REG_LENGTH];
    uint8_t i, found = 0;
    
    lcd_clear();
    lcd_str_write("Enter ROLL No ");
    lcd_cmd_write(0xC0);
    
    get_string(reg_no, REG_LENGTH);
    
    for(i = 0; i < MAX_STUDENTS; i++) {
        if(strcmp(reg_no, students[i].reg_no) == 0) {
            found = 1;
            if(!students[i].present) {
                students[i].present = 1;
                class_strength++;
                lcd_clear();
                lcd_str_write(students[i].name);
                lcd_cmd_write(0xC0);
                lcd_str_write("Marked Present!");
            } else {
                lcd_clear();
                lcd_str_write("Already");
                lcd_cmd_write(0xC0);
                lcd_str_write("Marked!");
            }
            break;
        }
    }
    
    if(!found) {
        buzz_error();
        lcd_clear();
        lcd_str_write("Invalid Roll No");
    }
    
    delay_ms(1500);
}

void handle_quiz(void) {
    uint8_t i;
    char answer;
    uint8_t score = 0;
    char score_str[16];
    
    for(i = 0; i < 3; i++) {
        lcd_clear();
        lcd_str_write(quiz_questions[i].question);
        lcd_cmd_write(0xC0);
        
        sprintf(score_str, "A:%s B:%s C:%s D:%s", 
                quiz_questions[i].options[0], 
                quiz_questions[i].options[1], 
                quiz_questions[i].options[2], 
                quiz_questions[i].options[3]);
        lcd_str_write(score_str);
        
        answer = get_key();
        
        if(answer == quiz_questions[i].correct_option) {
            score++;
            lcd_clear();
            lcd_str_write("Correct!");
        } else {
            buzz_error();
            lcd_clear();
            lcd_str_write("Wrong!");
        }
        delay_ms(1000);
    }
    
    lcd_clear();
    sprintf(score_str, "Score: %d/3", score);
    lcd_str_write(score_str);
    delay_ms(2000);
}

void handle_display(void) {
    char temp_str[16];
    char strength_str[16];
    
    current_temperature = read_temperature();
    sprintf(temp_str, "Temp: %dC", current_temperature);
    lcd_clear();
    lcd_str_write(temp_str);
    delay_ms(2000);
    
    lcd_clear();
    sprintf(strength_str, "Student Count:%d", class_strength);
    lcd_str_write(strength_str);
    delay_ms(2000);
    
    lcd_clear();
    lcd_str_write("Next Class ");
    lcd_str_write(get_random_time());
    lcd_cmd_write(0xC0);
    lcd_str_write(get_random_subject());
    delay_ms(2000);
}

void handle_emy(void) {
    int i;
    for(i = 0; i < 5; i++) {
        lcd_clear();
        lcd_str_write("Emergency Alert");
        LPC_GPIO1->FIOSET = ALL_LED;
        LPC_GPIO1->FIOSET = BUZZ;
        delay_ms(500);
        
        LPC_GPIO1->FIOCLR = ALL_LED;
        LPC_GPIO1->FIOCLR = BUZZ;
        delay_ms(500);
    }
}

void handle_exit(void) {
    // Turn off all devices
    LPC_GPIO1->FIOCLR = FAN | MOTOR | LIGHT | ALL_LED;
    fan_status = 0;
    light_status = 0;
    auto_fan_mode = 0;
    auto_light_mode = 0;
    
    lcd_clear();
    lcd_str_write("Shutting down...");
    
    LPC_GPIO1->FIOSET = BUZZ;
    delay_ms(500);
    LPC_GPIO1->FIOCLR = BUZZ;
    
    lcd_clear();
    lcd_str_write("System Off");
    delay_ms(1000);
    lcd_clear();
}

float read_temperature(void) {
    uint32_t adc_value;
    LPC_ADC->ADCR &= ~(0xFF << 8);
    LPC_ADC->ADCR |= (1<<2);
    LPC_ADC->ADCR |= (1<<24);
    while((LPC_ADC->ADDR2 & (1<<31)) == 0);
    adc_value = (LPC_ADC->ADDR2 >> 4) & 0xFFF;
    return (adc_value * 3.3 * 100.0) / 4096.0;
}

uint16_t read_ldr(void) {
    LPC_ADC->ADCR &= ~(0xFF << 8);
    LPC_ADC->ADCR |= (1<<3);  // Select AD0.3
    LPC_ADC->ADCR |= (1<<24); // Start conversion
    while((LPC_ADC->ADDR3 & (1<<31)) == 0);
    return (LPC_ADC->ADDR3 >> 4) & 0xFFF;
}

void initialize_students(void) {
uint32_t i;
    strcpy(students[0].name, "SUDHANSHU");
    strcpy(students[0].reg_no, "333");
    
    strcpy(students[1].name, "AYAN");
    strcpy(students[1].reg_no, "555");
    
    strcpy(students[2].name, "KAPISH");
    strcpy(students[2].reg_no, "444");
    
    strcpy(students[3].name, "NIRMAL");
    strcpy(students[3].reg_no, "222");
    
    strcpy(students[4].name, "ARPITA");
    strcpy(students[4].reg_no, "111");
    
    // Initialize presence to 0
	
    for( i = 0; i < MAX_STUDENTS; i++) {
        students[i].present = 0;
    }
}

char* get_random_subject(void) {
    return subjects[rand() % 5];
}

char* get_random_time(void) {
    return time[rand() % 5];
}

void keypad_init(void) {
    LPC_GPIO2->FIODIR |= ROW;
    LPC_GPIO2->FIODIR &= ~COL;
}

char get_key(void) {
    uint32_t i, j, col_val;
    
    while(1) {
        for(i = 0; i < 4; i++) {
            LPC_GPIO2->FIOCLR = ROW;
            LPC_GPIO2->FIOSET = code[i] << 4;
            col_val = LPC_GPIO2->FIOPIN & 0x0F;
            
            for(j = 0; j < 4; j++) {
                if(col_val == code[j]) {
                    delay_ms(20); // Debounce
                    return ktab[i][j];
                }
            }
        }
    }
}

void get_string(char *str, uint8_t max_len) {
    uint8_t i = 0;
    char key;
    
    while(i < max_len - 1) {
        key = get_key();
        if(key == '#') break;
        if(key >= '0' && key <= '9') {
            str[i++] = key;
            lcd_data_write(key);
        }
    }
    str[i] = '\0';
}

void lcd_cmd_write(char c) {
    LPC_GPIO0->FIOCLR = VP;
    LPC_GPIO0->FIOSET = c << 15;
    LPC_GPIO0->FIOCLR = RS;
    LPC_GPIO0->FIOSET = EN;
    delay_ms(10);
    LPC_GPIO0->FIOCLR = EN;
}

void lcd_data_write(char d) {
    LPC_GPIO0->FIOCLR = VP;
    LPC_GPIO0->FIOSET = d << 15;
    LPC_GPIO0->FIOSET = RS;
    LPC_GPIO0->FIOSET = EN;
    delay_ms(10);
    LPC_GPIO0->FIOCLR = EN;
}

void lcd_str_write(char *pstr) {
    while(*pstr != '\0') {
        lcd_data_write(*pstr);
        pstr++;
    }
}

void lcd_clear(void) {
    lcd_cmd_write(0x01);
    delay_ms(2);
}

void buzz_error(void) {
    LPC_GPIO1->FIOSET = BUZZ | LED;
    delay_ms(500);
    LPC_GPIO1->FIOCLR = BUZZ | LED;
}

void delay_ms(uint32_t ms) {
    uint32_t i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 1250; j++) {}
    }
}
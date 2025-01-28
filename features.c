
// functions.c
#include "smart_classroom.h"

// Keypad configuration
static uint32_t code[] = {0x0E, 0x0D, 0x0B, 0x07};
static char ktab[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

// Subject list for random selection
static char* subjects[] = {"MATH", "PHY", "CHEM", "ENG", "COMP"};

void lcd_init(void) {
    LPC_GPIO0->FIODIR |= RS | VP | EN;
    
    lcd_cmd_write(0x38); // 8 bit mode 2 line 
    lcd_cmd_write(0x0E); // cursor on display on
    lcd_cmd_write(0x01); // clear screen
    lcd_cmd_write(0x0C);
}

void show_menu(void) {
    lcd_clear();
    lcd_str_write("1:FA 2:FQ");
    lcd_cmd_write(0xC0);
    lcd_str_write("3:FD 4:FC");
}

void handle_attendance(void) {
    char reg_no[REG_LENGTH];
    uint8_t i, found = 0;
    
    lcd_clear();
    lcd_str_write("Enter Reg No:");
    lcd_cmd_write(0xC0);
    
    get_string(reg_no, 10);
    
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
        lcd_str_write("Invalid Reg No!");
    }
    
    delay_ms(1500);
}

void handle_quiz(void) {
    uint8_t i;
    char answer;
    uint8_t score = 0;
    
    for(i = 0; i < 3; i++) { // Only show first 3 questions
        lcd_clear();
        lcd_str_write(quiz_questions[i].question);
        lcd_cmd_write(0xC0);
        
        // Show options one by one with delay
        uint8_t j;
        for(j = 0; j < 4; j++) {
            lcd_cmd_write(0xC0);
            lcd_str_write(quiz_questions[i].options[j]);
            delay_ms(1000);
        }
        
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
    char score_str[16];
    sprintf(score_str, "Score: %d/3", score);
    lcd_str_write(score_str);
    delay_ms(2000);
}

void handle_display(void) {
    char temp_str[16];
    char strength_str[16];
    
    // Read and display temperature
    current_temperature = read_temperature();
    sprintf(temp_str, "Temp: %dC", current_temperature);
    lcd_clear();
    lcd_str_write(temp_str);
    
    // Display class strength
    lcd_cmd_write(0xC0);
    sprintf(strength_str, "Present: %d", class_strength);
    lcd_str_write(strength_str);
    delay_ms(2000);
    
    // Display next class
    lcd_clear();
    lcd_str_write("Next Class:");
    lcd_cmd_write(0xC0);
    lcd_str_write(get_random_subject());
    delay_ms(2000);
}

void handle_controls(void) {
    char key;
    lcd_clear();
    lcd_str_write("1:Fan 2:Light");
    lcd_cmd_write(0xC0);
    lcd_str_write("*:Back");
    
    while(1) {
        key = get_key();
        if(key == '1') {
            fan_status = !fan_status;
            if(fan_status)
                LPC_GPIO1->FIOSET = FAN;
            else
                LPC_GPIO1->FIOCLR = FAN;
        }
        else if(key == '2') {
            light_status = !light_status;
            if(light_status)
                LPC_GPIO1->FIOSET = LIGHT;
            else
                LPC_GPIO1->FIOCLR = LIGHT;
        }
        else if(key == '*')
            break;
            
        // Update status display
        lcd_clear();
        lcd_str_write(fan_status ? "Fan:ON " : "Fan:OFF ");
        lcd_str_write(light_status ? "L:ON" : "L:OFF");
        lcd_cmd_write(0xC0);
        lcd_str_write("*:Back");
    }
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
    
    while(i < max_len) {
        key = get_key();
        if(key == '#') break;
        if(key >= '0' && key <= '9') {
            str[i++] = key;
            lcd_data_write(key);
        }
    }
    str[i] = '\0';
}

void buzz_error(void) {
    LPC_GPIO1->FIOSET = BUZZ | LED;
    delay_ms(500);
    LPC_GPIO1->FIOCLR = BUZZ | LED;
}

float read_temperature(void) {
    LPC_ADC->ADCR |= (1<<24);
    while((LPC_ADC->ADDR2 & (1<<31)) == 0);
    uint32_t adc_value = (LPC_ADC->ADDR2 >> 4) & 0xFFF;
    return (adc_value * 3.3 * 100.0) / 4096.0;
}

void initialize_students(void) {
    // Initialize with 10 sample Indian names and registration numbers
    strcpy(students[0].name, "Raj Kumar");
    strcpy(students[0].reg_no, "2023000001");
    
    strcpy(students[1].name, "Priya Singh");
    strcpy(students[1].reg_no, "2023000002");
    
    strcpy(students[2].name, "Amit Patel");
    strcpy(students[2].reg_no, "2023000003");
    
    strcpy(students[3].name, "Neha Sharma");
    strcpy(students[3].reg_no, "2023000004");
    
    strcpy(students[4].name, "Rahul Gupta");
    strcpy(students[4].reg_no, "2023000005");
    
    strcpy(students[5].name, "Anita Verma");
    strcpy(students[5].reg_no, "2023000006");
    
    strcpy(students[6].name, "Suresh Kumar");
    strcpy(students[6].reg_no, "2023000007");
    
    strcpy(students[7].name, "Meena Devi");
    strcpy(students[7].reg_no, "2023000008");
    
    strcpy(students[8].name, "Arun Singh");
    strcpy(students[8].reg_no, "2023000009");
    
    strcpy(students[9].name, "Pooja Rani");
    strcpy(students[9].reg_no, "2023000010");
    
    // Initialize presence to 0
    for(int i = 0; i < MAX_STUDENTS; i++) {
        students[i].present = 0;
    }
}

char* get_random_subject(void) {
    return subjects[rand() % 5];
}

void delay_ms(uint32_t ms) {
    uint32_t i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 1250; j++) {}
    }
}

void lcd_clear(void) {
    lcd_cmd_write(0x01);
    delay_ms(2);
}
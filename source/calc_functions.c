#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> // need for snprintf
#include <math.h> // need for exponent
#include "../main.h"

// adds every input to the end of a master string
void input_to_str(char input, char* eq_str){
    if (eq_str[0] == '\0'){
        eq_str[0] = input;
    }else{
        for(int i = 0; eq_str[i]!='\0'; i++){
            if(eq_str[i+1] == '\0'){
                eq_str[i+1] = input;
                break;
            }
        }
    }
}

// returns index of the last char
int length_of_str(char* str){
    int32_t length = -1;
    for(int i = 0; str[i]!= '\0'; i++){
        length++;
    }
    return (0>length) ? 0 : length; // returns the greater number
}

// runs through checklist after a number is pressed
void num_pressed(I2C_LCD* lcd, uint8_t input_num, char* eq_str){
    input_to_str((char)(input_num + 48), eq_str);// (input_num + 48) is for ascii
    
    lcd_print_char(lcd, (char)(input_num + 48));

    delay_SysTick(400, lcd->sys_freq);
}

// runs through checklist after an operator is pressed
void operator_pressed(I2C_LCD* lcd, char op, char* eq_str){
    
    char last_char = eq_str[length_of_str(eq_str)];
    if((last_char<'0' || last_char>'9') && last_char != '\0'){
        eq_str[length_of_str(eq_str)] = '\0';

        lcd->current_col--;
        lcd_set_cursor(lcd);
    }

    input_to_str(op, eq_str);

    lcd_print_char(lcd, op);

    if(op == '='){
        char ans_str[20];
        snprintf(ans_str, sizeof(ans_str), "%.3g", str_to_ans(eq_str));

        // set location for answer
        lcd->current_row++;
        lcd->current_col = lcd->num_cols - (length_of_str(ans_str)+1);
        lcd_set_cursor(lcd);

        lcd_print_string(lcd, ans_str);
        
        free(eq_str);
        eq_str = calloc(arr_length, sizeof(char));
    }
    delay_SysTick(400, lcd->sys_freq);
}

// takes master equation string and solves it
double str_to_ans(char* eq_str){

    double* numbers = calloc(arr_length, sizeof(double));
    int* operator_index = calloc(arr_length, sizeof(int));
    int j = 0;



    // put all elements into an array
    //* goes number operator number ...
    for(int i = 0; eq_str[i] != '\0'; i++){
        if(eq_str[i]<'0' || eq_str[i]>'9'){
            operator_index[j] = i;
            j++;
        }else{
            numbers[j] = (numbers[j] * 10) + ((int) eq_str[i]) - 48;
        }
    }

    int current_ops = j;// tells how many operations there are


    // with pemdas
    for(int i = 0; i<current_ops; i++){
        if(eq_str[operator_index[i]] == '^'){  

            // preform operation
            numbers[i] = pow(numbers[i], numbers[i+1]);    
            // shift array down towards 0
            for(int j = i; j<(current_ops-1); j++){
                numbers[j+1] = numbers[j+2];
                operator_index[j] = operator_index[j+1];
            }      
            current_ops--;
            i--;

        }
    }

    for(int i = 0; i<current_ops; i++){
        if(eq_str[operator_index[i]] == '*'){

            // preform operation
            numbers[i] = numbers[i] * numbers[i+1];

            // shift array down towards 0
            for(int j = i; j<(current_ops-1); j++){
                numbers[j+1] = numbers[j+2];
                operator_index[j] = operator_index[j+1];
            }

            current_ops--;
            i--;
            
        }else if (eq_str[operator_index[i]] == '/'){

            // preform operation
            numbers[i] = numbers[i] / numbers[i+1];

            // shift array down towards 0
            for(int j = i; j<(current_ops-1); j++){
                numbers[j+1] = numbers[j+2];
                operator_index[j] = operator_index[j+1];
            }

            current_ops--;
            i--;
        }
        
    }

    for(int i = 0; i<current_ops; i++){
        if(eq_str[operator_index[i]] == '+'){

            // preform operation
            numbers[i] = numbers[i] + numbers[i+1];

            // shift array down towards 0
            for(int j = i; j<(current_ops-1); j++){
                numbers[j+1] = numbers[j+2];
                operator_index[j] = operator_index[j+1];
            }

            current_ops--;
            i--;
            
        }else if (eq_str[operator_index[i]] == '-'){

            // preform operation
            numbers[i] = numbers[i] - numbers[i+1];

            // shift array down towards 0
            for(int j = i; j<(current_ops-1); j++){
                numbers[j+1] = numbers[j+2];
                operator_index[j] = operator_index[j+1];
            }

            current_ops--;
            i--;
        }
    }

    double answer = numbers[0];

    free(numbers);
    free(operator_index);

    return answer;
}
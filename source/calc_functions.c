#include <stdint.h>
#include <stdlib.h>
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

// runs through checklist after a number is pressed
void num_pressed(uint8_t input_num, char* eq_str){
    input_to_str((char)(input_num + 48), eq_str);
    // (input_num + 48) is for ascii
    lcd_send_data((char)(input_num + 48));
    delay_SysTick(200);
}

// runs through checklist after an operator is pressed
void operator_pressed(char op, char* eq_str){
    input_to_str(op, eq_str);
    lcd_send_data(op);
    delay_SysTick(200);
}

// takes master equation string and solves it
double str_to_ans(char* eq_str){
    double numbers[10];
    int* operator_index = calloc(10, sizeof(int));
    int j = 0;



    // put all elements into an array
    //* goes number operator number ...
    for(int i = 0; eq_str[i] != '\0'; i++){
        if(eq_str[i]<'0' || eq_str[i]>'9'){
            operator_index[j] = i;
            j++;
        }else{
            numbers[j] = (numbers[j] * 10) + ((int) eq_str[i]);
        }
    }

    // TODO: just a test without pemdas
    double answer = numbers[0];
    for(int k = 0; operator_index[k] != 0; k++){
        switch(eq_str[operator_index[k]]){
        case '+':
            answer += numbers[k+1];
            break;
        case '-':
            answer -= numbers[k+1];
            break;
        case '*':
            answer *= numbers[k+1];
            break;
        case '/':
            answer /= numbers[k+1];
            break;
        default:
            break;
        }
    }

    return answer;
}
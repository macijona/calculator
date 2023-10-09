#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char operators [] = "/+*-";
float ans = 0;

void introduce_calculator(){
    printf("\nThis is simple calculator, only can use digits, / * + -, and previous answer (ans)\n");
    printf("The calculator will use order of operations to compute the answer\n");
    printf("To quit, type 'quit'\n");
    printf("Calc: ");
}

bool is_part_of_ans(char * str, int i){
    if(i<strlen(str)-2 && str[i] == 'a' && str[i+1] == 'n' && str[i+2] == 's'){
        return true;
    }
    else if(i>0 && i<strlen(str)-1 && str[i-1]=='a' && str[i] == 'n' && str[i+1]=='s'){
        return true;
    }
    else if(i>1 && str[i-2]=='a' && str[i-1] == 'n' && str[i]=='s'){
        return true;
    }
    else{
        return false;
    }
}

bool format_is_good(char* str, bool allow_ans){
    // remove all spaces in string
    int i = 0, j = 0;
    char * buffer = malloc(1000);
    while(str[i] != '\0'){
        if(str[i] != ' '){
            buffer[j] = str[i];
            j++;
        }
        i++;
    }
    buffer[j] = '\0';
    strcpy(str, buffer);
   free(buffer);
    
    // ensure that only characters in str are numbers, /, *, +, -, and "ans"
    bool cur_i_digit = false;
    bool cur_i_operator = false;
    bool cur_i_ans = false;
    bool contains_ans = false;
    for(int i = 0; i < strlen(str); i++){
        if(isdigit(str[i])){
            // if the previous character was part of 'ans', error out
            if(cur_i_ans){
                return false;
            }
            cur_i_digit = true;
            cur_i_operator = false;
            cur_i_ans = false;
        }
        else if(strchr(operators, str[i]) != NULL){
            // op must only happen when betwen digits and 'ans', not after another op
            if(cur_i_operator){
                return false;
            }
            cur_i_digit = false;
            cur_i_operator = true;
            cur_i_ans = false;
        }
        else if(is_part_of_ans(str, i)){
            contains_ans = true;
            // ans must only happen when betwen ops, not after a digit or another ans
            if(cur_i_digit){
                return false;
            }
            // check if next 3 digits are part of ans, which means ansans must have occured:
            if(i<strlen(str)-3 && is_part_of_ans(str, i+1) && is_part_of_ans(str, i+2) && is_part_of_ans(str, i+3)){
                return false;
            }
            cur_i_digit = false;
            cur_i_operator = false;
            cur_i_ans = true;
        }
        else{
            return false;
        }

    }

    // ensure that first or last character are not operators, meaning each operator has operands
    if(strchr(operators, str[0]) != NULL || strchr(operators, str[strlen(str)-1]) != NULL){
        return false;
    }
    if(!allow_ans&&contains_ans){
        return false;
    }
 
    return true;
}

void replace_ans(char* str, float ans){
    // replace all instances of 'ans' with the previous answer
    char * buffer = malloc(1000);
    int i = 0, j = 0;
    while(str[i] != '\0'){
        if(is_part_of_ans(str, i)){
            sprintf(buffer+j, "%f", ans);
            j += strlen(buffer+j);
            i += 3;
        }
        else{
            buffer[j] = str[i];
            j++;
            i++;
        }
    }
    buffer[j] = '\0';
    strcpy(str, buffer);
    free(buffer);
}

float value(char* str){
    // check if operand is a number or 'ans'
    if(is_part_of_ans(str, 0)){
        return ans;
    }
    else{
        return atof(str);
    }
}

float compute_ans(float operand1, float operand2, char operation){
    switch(operation){
        case '*':
            return operand1 * operand2;
        case '/':
            return operand1 / operand2;
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
    }
    return 0;
}

int compute_and_replace(char *str, char* operand1, char* operand2, int op1_len, int op2_len, char operation){
    char* operand1_buf = malloc(1000);
    char* operand2_buf = malloc(1000);
    memset(operand1_buf, 0, 1000);
    memset(operand2_buf, 0, 1000);
    strncpy(operand1_buf, operand1, op1_len);
    strncpy(operand2_buf, operand2, op2_len);
    float op1 = value(operand1_buf);
    float op2 = value(operand2_buf);

    // compute the result of the operation
    float result = compute_ans(op1, op2, operation);

    // update str to replace operand1 op operand2 with result
    char* result_buf = malloc(1000);
    sprintf(result_buf, "%f", result);

    int result_buf_len = strlen(result_buf);
    int str_len = strlen(str);
    int operation_len = op1_len + op2_len + 1;

    // shift_size is the size of the components after the operation/result
    int shift_size = str_len - (operand2 - str) - op2_len;

    // if operation_len >= result_buf_len, then its shifted to the left, else its shifted to the right
    memmove(operand1 + result_buf_len, operand1 + operation_len, shift_size);
    memmove(operand1, result_buf, result_buf_len);

    // update the length of str
    str[str_len - operation_len + result_buf_len] = '\0';
    free(operand1_buf);
    free(operand2_buf);
    free(result_buf);
    return result_buf_len;
}

void compute_ops(char* str, char* ops){
    // find first substring before an ops operator
    char* operand1 = str, *operand2 = str;
    char operation;
    int i = 0;
    while(str[i] != '\0'){
        if(strchr(ops, str[i]) != NULL){
            operation = str[i];
            // first operand is from operand1 to str[i], second is first char after str[i] to next operator
            int op1_len = i - (operand1 - str);
            operand2 = str + i + 1;
            int op2_len = 0;
            int j = i+1;
            while(strchr(operators, str[j]) == NULL){
                j++;
                op2_len++;
            }

            int return_sz = compute_and_replace(str, operand1, operand2, op1_len, op2_len, operation);
            //NOTE: operand 1 still points to beggining of the result, so its still current operand for next time
            i = operand1 - str + return_sz;

        }
        else{ // this was not an ops operator, so continue to next character
            // if the current char is a non-ops operator,
            // then the next character is the start of the potential next operand
            if(strchr(operators, str[i]) != NULL){
                operand1 = str + i + 1;
            }
            i++;
        }
    }
}

int main(){
    // Collect string from user
    char *str = malloc(1000);
    bool history_exists = false;
    while(true){
        // grab input with fgets()
        introduce_calculator();
        fgets(str, 1000, stdin);
        if ((strlen(str) > 0) && (str[strlen (str) - 1] == '\n'))
            str[strlen (str) - 1] = '\0';

        // Check if user wants to quit
        if(strcmp(str, "quit") == 0){
            printf("Goodbye!\n");
            free(str);
            return 0;
        }
        // check if format of input is correct
        if(!format_is_good(str, history_exists)){
            printf("Invalid input!!\n");
        }
        else{
            printf("    str: %s\n", str);
            compute_ops(str,"*/");
            printf("    str after */ operations: %s\n", str);
            compute_ops(str,"+-");
            printf("    str after +- operations: %s\n", str);
            ans = atof(str);
            printf("ans: %f\n", ans);
            printf("'ans' stored..\n");
            history_exists |= true;
        }
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define IS_NEGATIVE(c) (c == '-')
#define IS_NEG_TOKEN(c) (c == neg_token)

char operators [] = "/+*-";
char non_front_operators [] = "/+*";
char neg_token = '~';
float ans = 0;

/**
 * @brief introduction to the calculator app
 * 
 */
void introduce_calculator(){
    printf("\nThis is simple calculator, only can use digits, / * + -, and previous answer (ans)\n");
    printf("The calculator will use order of operations to compute the answer\n");
    printf("To quit, type 'quit'\n");
    printf("Calc: ");
}

/**
 * @brief Returns true if the character at str[i] is part of the word 'ans'
 * 
 * @param str 
 * @param i 
 * @return true or false
 */
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

/**
 * @brief Check the format of the user input. Removing spaces, checking expression logic and replacing negative sign with negative token.
 * 
 * @param str 
 * @param allow_ans 
 * @return true or false
 */
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
    bool allow_neg = true;
    bool contains_ans = false;
    for(int i = 0; i < strlen(str); i++){
        // digit: if the previous character was part of 'ans', error out
        if(isdigit(str[i])){
            if(cur_i_ans){
                return false;
            }
            cur_i_digit = true;
            cur_i_operator = false;
            cur_i_ans = false;
        }
        // op: must only happen betwen digits and 'ans', not after another op
        else if(strchr(operators, str[i]) != NULL){
            // Exception: negative sign is allowed after another operator
            bool is_negative = IS_NEGATIVE(str[i]);
            if(cur_i_operator){
                if(allow_neg && is_negative){
                    // replace symbol with negative token
                    str[i] = neg_token;
                    allow_neg = false;
                }
                else{
                    return false;
                }
            }
            else{
                // if this is a negative sign in the beggining, only allow one negative sign
                if(i == 0 && is_negative){
                    // replace symbol with negative token
                    str[0] = neg_token;
                    allow_neg = false;
                }
                else{
                    allow_neg = true;
                }
            }
            cur_i_digit = false;
            cur_i_operator = true;
            cur_i_ans = false;
        }
        // ans: must only happen betwen ops, not after a digit or another ans
        else if(is_part_of_ans(str, i)){
            contains_ans = true;
            if(cur_i_digit){
                return false;
            }
            // check if next 3 digits are part of ans, which means 'ansans' must have occured:
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


/**
 * @brief Returns the value of the operand str, which is either a number or 'ans'
 * 
 * @param str 
 * @return float 
 */
float value(char* str){
    // check if operand is a number or 'ans'
    int str_sign = 1;
    if(IS_NEG_TOKEN(str[0])){
        str[0] = '-'; // replace negative token with negative sign
        str_sign = -1;
    }

    if(is_part_of_ans(str, 1)){
        return str_sign * ans;
    }
    else{
        return atof(str);
    }
}

/**
 * @brief computes the operation given: operand1 operation operand2
 * 
 * @param operand1 
 * @param operand2 
 * @param operation 
 * @return float 
 */
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

/**
 * @brief Computes the result of the operation and replaces the operation with the result in str
 * 
 * @param str 
 * @param operand1 
 * @param operand2 
 * @param op1_len 
 * @param op2_len 
 * @param operation 
 * @return int 
 */
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

    // replace netagive sign with negative token
    if (result < 0){
        result_buf[0] = neg_token;
    }

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

/**
 * @brief Computes all operations in str that are in ops
 * 
 * @param str 
 * @param ops 
 */
void compute_ops(char* str, char* ops){
    // find first substring before an ops operator
    char* operand1 = str, *operand2 = str;
    char operation;
    int i = 0;
    while(str[i] != '\0'){
        // if this is an op character
        if(strchr(ops, str[i]) != NULL){
            operation = str[i];
            // first operand is from operand1 to str[i], second is first char after str[i] to next operator
            int op1_len = i - (operand1 - str);
            operand2 = str + i + 1;
            int op2_len = 0;
            int j = i+1;
            // where there is no operator, continue to next character
            while(strchr(operators, str[j]) == NULL){ // || (IS_NEG_TOKEN(str[j])&&j==i+1)
                j++;
                op2_len++;
            }

            int return_sz = compute_and_replace(str, operand1, operand2, op1_len, op2_len, operation);
            //NOTE: operand 1 still points to beggining of the result, so its still current operand for next time
            i = operand1 - str + return_sz;

        }
        // this was not an ops operator, so continue to next character
        else{
            // if the current char is another op,
            // then the next character is the start of the potential next operand
            if(strchr(operators, str[i]) != NULL)
                operand1 = str + i + 1;
            i++;
        }
    }
}

/**
 * @brief cleans up str by replace tokens with their real symbols
 * 
 * @param str 
 * @return char* 
 */
char * clean(char * str){
    int i = 0;
    while(str[i] != '\0'){
        if(IS_NEG_TOKEN(str[i]))
            str[i] = '-';
        i++;
    }
    return str;
}

/**
 * @brief Asks user for a math expression and computes the result of the expression
 */
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
        if(!format_is_good(str, history_exists))
            printf("Invalid input!!\n");
        else{
            printf("    str: %s\n", str);
            compute_ops(str,"*/");
            printf("    str after */ operations: %s\n", str);
            compute_ops(str,"+-");
            printf("    str after +- operations: %s\n", str);
            ans = atof(clean(str));
            printf("ans: %f\n", ans);
            printf("'ans' stored..\n");
            history_exists |= true;
        }
    }
}


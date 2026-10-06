/**
 * Filename : Assignment1(a)
 * Description :Calculator Problem Statement
 * Author : Nancy Jain
 */

#include<stdio.h>

#define MAX 100

int parseExpression(const char *expression , int operands[] , char ops[] , int *count){
    int pos = 0 , used;

    if(sscanf(expression + pos , "%d%n" , &operands[0], &used) != 1){
        return 1;
    }
    pos += used;
    *count = 1;
    while(sscanf(expression + pos , " %c%n" , &ops[*count - 1] , &used) == 1){
        pos += used;
        if(ops[*count - 1] != '+' && ops[*count - 1] != '-' && ops[*count - 1] != '*' && ops[*count - 1] != '/'){
            return 1;
        }
        if(sscanf(expression + pos , "%d%n" , &operands[*count] , &used) != 1){
            return 1;
        }
        pos += used;
        (*count)++;
    }
    return 0;
}

int applyOperator(int a , int b , char op , int *result){
    switch(op){
        case '+':
            *result = a + b;
            break;
        case '-':
            *result = a - b;
            break;
        case '*':
            *result = a * b;
            break;
        case '/':
            if(b == 0){
                return 2;
            }
            *result = a / b;
            break;
        default:
            return 1;
    }
    return 0;
}

int evaluate(int operands[] , char ops[] , int count , int *result){
    int i = 0 , j , status;

    while(i < count - 1){
        if(ops[i] == '*' || ops[i] == '/'){
            status = applyOperator(operands[i] , operands[i+1] , ops[i] , &operands[i]);
            if(status != 0){
                return status;
            }
            for(j = i + 1 ; j < count - 1 ; j++){
                operands[j] = operands[j + 1];
                ops[j - 1] = ops[j];
            }
            count--;
        }
        else{
            i++;
        }
    }

    *result = operands[0];
    for(i = 0 ; i < count - 1 ; i++){
        status = applyOperator(*result , operands[i+1] , ops[i] , result);
        if(status != 0){
            return status;
        }
    }
    return 0;
}

int main(){
    char expression[MAX] , ops[MAX];
    int operands[MAX];
    int count = 0 , result = 0 , status;
    if(fgets(expression , MAX , stdin) == NULL){
        printf("Error: Invalid expression.\n");
        return 0;
    }
    
    status = parseExpression(expression , operands , ops , &count);
    if(status == 0){
        status = evaluate(operands , ops , count , &result);
    }

    if(status == 1){
        printf("Error: Invalid expression.\n");
    }else if(status == 2){
        printf("Error: Division by zero.\n");
    }else{
        printf("%d\n", result);
    }
    return 0;
}

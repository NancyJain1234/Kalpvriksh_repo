/**
 * Filename : Assignment1(a)
 * Description :Calculator Problem Statement
 * Author : Nancy Jain
 */

#include<stdio.h>

#define MAX 100

int getInt(char message[]){
    int num;
    char ch = 0;
    while(1){
        printf("%s", message);
        if(scanf("%d%c", &num, &ch) == 2 && ch == '\n' && num > 0){
            return num;
        }
        printf("Invalid input. Enter numbers only.\n");
        while(ch != '\n'){
            ch = getchar();
        }
        ch = 0;
    }
}

int main(){
    int choice;
    do{
        printf("\nPress 1 to enter expression:\n");
        printf("Enter 2 to exit.\n");
        choice = getInt("Enter your choice: ");
        switch(choice){
            case 1:
            {
                char expression[MAX] , operator[MAX];
                int operands[MAX];
                int count = 0 , pos = 0 , used , i , j ;
                printf("Enter the expression : ");
                if(fgets(expression , MAX , stdin) == NULL){
                    printf("Error: Invalid Expression.");
                    return 0;
                }
                if(sscanf(expression + pos , "%d%n" , &operands[count] , &used) != 1){
                    printf("Error: Invalid Expression.");
                    return 0;
                }

                pos += used;
                count = 1;
                while(sscanf(expression + pos , " %c%n" , &operator[count - 1] , &used) == 1){
                    pos += used;
                    if(operator[count - 1] != '+' && operator[count - 1] != '-' && operator[count - 1] != '*' && operator[count - 1] != '/'){
                        printf("Error: Invalid Expression.");
                        return 0;
                    }
                    if(sscanf(expression + pos , "%d%n" , &operands[count] , &used) != 1){
                        printf("Error: Invalid Expression.");
                        return 0;
                    }
                    pos += used;
                    count++;
                    i = 0;
                    while(i < count - 1){
                        if(operator[i] == '*' || operator[i] == '/'){
                            if(operator[i] == '*'){
                                operands[i] = operands[i] * operands[i + 1];
                            }else{
                                if(operands[i + 1] == 0){
                                    printf("Error: Division by zero.");
                                    return 0;
                                }
                                operands[i] = operands[i] / operands[i + 1];
                            }
                            for(j = i + 1 ; j < count - 1 ; j++){
                                operands[j] = operands[j + 1];
                                operator[j - 1] = operator[j];
                            }
                            count--;
                        }
                        else{
                            i++;
                        }
                    }
                }
                int result = operands[0];
                for(i = 0 ; i < count - 1 ; i++){
                    if(operator[i] == '+'){
                        result += operands[i + 1];
                    }
                    else{
                        result -= operands[i + 1];
                    }
                }
                printf("Result: %d\n", result);
                break;
            }
            case 2:
            {
                printf("Exiting.");
                break;
            }
            default:
                printf("Invalid Choice.");
                break;
        }
    }while(choice != 2);
    return 0;
}

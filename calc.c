/**
 * FILENAME - calc.c
 * DESCRIPTION - string of complex no. and create a menu based program.
        1) for addition
        2) for subtraction 
        3) for division
        4) for multiplication 
        5) Finding factor of equations.
  * AUTHOR - Nancy Jain
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define SIZE 10

void parse(char s[], int c[]){
    int i = 0 , p;
    for(p = 0 ; p < SIZE ; p++) c[p] = 0;
    while(s[i] != '\0'){
        int sign = 1 , num = 1 , power = 0;
        while(s[i] == ' '){
            i++;
        }
        if(s[i] == '\0') break;
        int begin = i;
        if(s[i] == '+') i++;
        else if(s[i] == '-'){
            sign = -1;
            i++;
        }
        while(s[i] == ' '){
            i++;
        }
        if(isdigit(s[i])){
            num = 0;
            while(isdigit(s[i])){
                num = num * 10 + s[i] - '0';
                i++;
            }
        }
        while(s[i] == ' '){
            i++;
        }
        if(s[i] == 'x' || s[i] == 'X'){
            power = 1;
            i++;
            if(s[i] == '^'){
                i++;
                power = 0;
                if(isdigit(s[i])){
                    power = s[i] - '0';
                    i++;
                }
            }
        }
        if(i == begin){
            i++;
            continue;
        }
        if(power < SIZE) c[power] += sign * num;
    }
}

int getDegree(int c[]){
    for(int i = SIZE - 1; i >= 0 ; i--){
        if(i > 0 && c[i] != 0){
            return i;
        }
    }
    return 0;
}

void show(int c[]){
    for(int i = getDegree(c) ; i >= 0 ; i--){
        if(c[i] == 0) continue;
        else if(c[i] < 0){
            printf(" -");
        }else{
            printf(" +");
        }
        if(abs(c[i]) != 1 || i == 0){
            printf("%d", abs(c[i]));
        }
        if(i >= 1) printf("x");
        if(i >= 2) printf("^%d" , i);
    }
    printf("\n");
}

void add(int A[] , int B[]){
    int c[SIZE];
    for(int i = 0 ; i < SIZE ; i++){
        c[i] = A[i] + B[i];
    }
    show(c);
}

void subtract(int A[] , int B[]){
    int c[SIZE];
    for(int i = 0 ; i < SIZE ; i++){
        c[i] = A[i] - B[i];
    }
    show(c);
}

void multiply(int A[] , int B[]){
    if(getDegree(A) + getDegree(B) >= SIZE){
        printf("Error: Overflow.");
        return;
    }
    int c[SIZE] = {0};
    for(int i = 0 ; i <= getDegree(A) ; i++){
        for(int j = 0 ; j <= getDegree(B) ; j++){
            c[i + j] += A[i] * B[j];
        }
    }
    show(c);
}

void divide(int A[] , int B[]){
    int q[SIZE] = {0} , rem[SIZE];
    for(int i = 0 ; i < SIZE ; i++){
        rem[i] = A[i];
    }
    int degA = getDegree(A) , degB = getDegree(B);
    if(degB == 0 && B[0] == 0){
        printf("Error: cannot divide by 0");
        return;
    }
    for(int i = degA - degB ; i >= 0 ; i--){
        q[i] = rem[i + degB] / B[degB];
        for(int j = 0 ; j <= degB ; j++){
            rem[i + j] -= q[i] * B[j];
        }
    }
    show(q);
    show(rem);
}

void showFactors(double r){
    if(r >= 0){
        printf("(x - %.2f)" , r);
    }else{
        printf("(x + %.2f)" , -r);
    }
}

void getFactors(int c[]){
    int degree = getDegree(c);
    if(degree == 1){
        printf("%d" , c[1]);
        showFactors(-(double)c[0] / c[1]);
        printf("\n");
    }else if(degree == 2){
        double d = c[1] * c[1] - 4 * c[2] * c[0];
        if(d < 0){
            printf("Imaginary roots.");
        }else{
            double r1 = (-c[1] + sqrt(d)) / (2 * c[2]);
            double r2 = (-c[1] - sqrt(d)) / (2 * c[2]);
            printf("%d" , c[2]);
            showFactors(r1);
            showFactors(r2);
            printf("\n");
        }
    }else{
        printf("Only for degree 1 and 2");
    }
}

int main(){
    char s1[100] , s2[100];
    int A[SIZE] , B[SIZE];
    int choice;
    fgets(s1 , sizeof(s1) , stdin);
    s1[strcspn(s1 , "\n")] = '\0';
    fgets(s2, sizeof(s2) , stdin);
    s2[strcspn(s2 , "\n")] = '\0';
    parse(s1 , A);
    parse(s2 , B);

    printf("Enter \n1. Addition\n2. Subtraction \n3.Multiplication\n4.Division\n5.Finding Factors\n");
    printf("Enter Choice:\n");
    scanf("%d" , &choice);
    switch(choice){
        case 1:{
            add(A , B);
            break;
        }
        case 2:{
            subtract(A , B);
            break;
        }
        case 3:{
            multiply(A , B);
            break;
        }
        case 4:{
            divide(A, B);
            break;
        }
        case 5:{
            getFactors(A);
            getFactors(B);
            break;
        }
        default:{
            printf("Error: invalid choice.");
        }
    }
    return 0;
}

/**
 * Filename : Assignment1.c
 * Description : Design a console-based Student Performance Analyzer program.
 * Author : Nancy Jain
 */

#include<stdio.h>
#include<string.h>

#define N 100
#define MAX_LEN 50
#define MAX_MARKS 100
#define SUBJECTS 3
#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

struct Student{
    int roll_no;
    char name[MAX_LEN];
    int marks[SUBJECTS];
};

int total(const int marks[]){
    int sum = 0;
    for(int i = 0 ; i < SUBJECTS ; i++){
        sum += marks[i];
    }
    return sum;
}

float average(int sum){
    return (float)sum / SUBJECTS;
}

int isValid(const int marks[]){
    for(int i = 0 ; i < SUBJECTS ; i++){
        if(marks[i] < 0 || marks[i] > MAX_MARKS){
            return 0;
        }
    }
    return 1;
}

char getGrade(float avg){
    if(avg >= GRADE_A){
        return 'A';
    }else if(avg >= GRADE_B){
        return 'B';
    }else if(avg >= GRADE_C){
        return 'C';
    }else if(avg >= GRADE_D){
        return 'D';
    }
    return 'F';
}

int starCount(char grade){
    switch(grade){
        case 'A':    return 5;
        case 'B':    return 4;
        case 'C':    return 3;
        case 'D':    return 2;
        default:     return 0;
    }
}

void printPerformance(char grade){
    int stars = starCount(grade);
    printf("Performance: ");
    for(int i = 0 ; i < stars ; i++){
        printf("*");
    }
    printf("\n");
}

void printStudent(const struct Student *student , int sum , float avg , char grade){
    printf("Roll: %d\n", student->roll_no);
    printf("Name: %s\n", student->name);
    printf("Total: %d\n", sum);
    printf("Average: %.2f\n", avg);
    printf("Grade: %c\n" , grade);
}

void printRolls(const struct Student student[] , int count , int index){
    if(index == count){
        return;
    }
    printf("%d" , student[index].roll_no);
    if(index < count - 1){
        printf(" ");
    }
    printRolls(student , count , index + 1);
}

int main(){
    struct Student student[N];
    int n;
    char line[200];
    if(scanf("%d", &n) != 1 || n < 1 || n > N){
        printf("Invalid input.\n");
        return 0;
    }

    int ch;
    while((ch = getchar()) != '\n' && ch != EOF){
        if(ch != ' ' && ch != '\t' && ch != '\r'){
            printf("Invalid input.");
            return 0;
        }
    }

    for(int i = 0 ; i < n ; i++){
        struct Student *s = &student[i];
        if(fgets(line , sizeof(line) , stdin) == NULL){
            printf("Invalid input.\n");
            return 0;
        }
        int pos = 0;
        int used = 0;

        if(sscanf(line , "%d %n" , &s->roll_no , &pos) != 1 || s->roll_no <= 0){
            printf("Invalid input.\n");
            return 0;
        }
        if(sscanf(line + pos , "%49[A-Za-z ] %d %d %d %n", s->name, &s->marks[0] , &s->marks[1] , &s->marks[2] , &used) != 4){
            printf("Invalid input.\n");
            return 0;
        }
        int len = strlen(s->name);
        while(len > 0 && s->name[len - 1] == ' '){
            s->name[--len] == '\0';
        }
        char *p = line + pos + used;
        while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            p++;
        }
        if(*p != '\0' || s->name[0] == '\0' || !isValid(s->marks)){
            printf("Invalid input.\n");
            return 0;
        }
    }

    for(int i = 0 ; i < n ; i++){
        int sum = total(student[i].marks);
        float avg = average(sum);
        char grade = getGrade(avg);
        printStudent(&student[i] , sum , avg , grade);
        if(avg >= GRADE_D){
            printPerformance(grade);
        }
    }

    printf("List of Roll Numbers (via recursion): ");
    printRolls(student , n , 0);
    printf("\n");
    return 0;
}
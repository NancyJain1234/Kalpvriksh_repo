/**
 * Filename : Assignment1.c
 * Description : Design a console-based Student Performance Analyzer program.
 * Author : Nancy Jain
 */

#include<stdio.h>

#define N 100
#define MAX_LEN 50
#define MAX_MARKS 100

struct Student{
    int roll_no;
    char name[MAX_LEN];
    int marks1;
    int marks2;
    int marks3;
};

int studentCount = 0;

int total(int marks1 , int marks2 , int marks3){
    return (marks1 + marks2 + marks3);
}

float average(int marks1 , int marks2 , int marks3){
    int sum = total(marks1 , marks2 , marks3);
    return (float)sum/3;
}

char getGrade(float avg){
    if(avg >= 85){
        return 'A';
    }else if(avg >= 70){
        return 'B';
    }else if(avg >= 50){
        return 'C';
    }else if(avg >= 35){
        return 'D';
    }
    return 'F';
}

int starCount(char grade){
    switch(grade){
        case 'A':{
            return 5;
        }
        case 'B':{
            return 4;
        }
        case 'C':{
            return 3;
        }
        case 'D':{
            return 2;
        }
        default:{
            return 0;
        }
    }
}

void printStudent(struct Student *student , float avg , char grade){
    printf("Roll: %d\n", student->roll_no);
    printf("Name: %s\n", student->name);
    int sum = total(student->marks1 , student->marks2 , student->marks3);
    printf("Total: %d\n", sum);
    printf("Average: %.2f\n", avg);
    printf("Grade: %c\n" , grade);
}

void printRolls(struct Student student[] , int index){
    if(index == studentCount){
        return;
    }
    printf("%d" , student[index].roll_no);
    if(index < studentCount - 1){
        printf(" ");
    }
    printRolls(student , index + 1);
}

int main(){
    struct Student student[N];
    int n;
    if(scanf("%d", &n) != 1 || n < 1 || n > N){
        printf("Invalid input.\n");
        return 0;
    }

    for(int i = 0 ; i < n ; i++){
        struct Student *s = &student[i];
        if(scanf("%d %49s %d %d %d" , &s->roll_no , s->name , &s->marks1 , &s->marks2 , &s->marks3) != 5){
            printf("Invalid input.\n");
            return 0;
        }
        if(s->marks1 < 0 || s->marks1 > MAX_MARKS || s->marks2 < 0 || s->marks2 > MAX_MARKS || s->marks3 < 0 || s->marks3 > MAX_MARKS){
            printf("Invalid input.\n");
            return 0;
        }
        studentCount++;
    }
    for(int i = 0 ; i < studentCount ; i++){
        float avg = average(student[i].marks1 , student[i].marks2 , student[i].marks3);
        char grade = getGrade(avg);
        printStudent(&student[i] , avg , grade);
        if(avg < 35){
            continue;
        }
        printf("Performance: ");
        for(int i = 0 ; i < starCount(grade) ; i++){
            printf("*");
        }
        printf("\n");
    }

    printf("List of Roll Numbers (via recursion): ");
    printRolls(student , 0);
    printf("\n");
    return 0;
}
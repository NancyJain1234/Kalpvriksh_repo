/**
 * FILENAME: Hierarchy.c
 * DESCRIPTION: Develop a C program to manage an organization's employee hierarchy.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMP 100
#define SIZE 100

struct Employee{
    int id;
    char name[SIZE];
    char designation[SIZE];
    struct Employee *manager;
    struct Employee *reports[MAX_EMP];
    int reportCount;
};
struct Employee *ceo = NULL;
int employeeCount = 0;

void clearBuffer(){
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF){}
}

struct Employee *searchTree(struct Employee *employee , int id){
    if(employee == NULL) return NULL;
    if(employee->id == id) return employee;
    for(int i = 0 ; i < employee->reportCount ; i++){
        struct Employee *found = searchTree(employee->reports[i] , id);
        if(found != NULL) return found;
    }
    return NULL;
}

struct Employee *findEmp(int id){
    return searchTree(ceo , id);
}

void freeTree(struct Employee *employee){
    if(employee == NULL) return;
    for(int i = 0 ; i < employee->reportCount ; i++){
        freeTree(employee->reports[i]);
    }
    free(employee);
}

void addEmployee(){
    int id , managerID;
    char name[SIZE] , designation[SIZE];
    struct Employee *manager = NULL;
    if(employeeCount >= MAX_EMP){
        printf("organization full.\n");
        return;
    }
    printf("Enter details:\n");
    if(scanf("%d", &id) != 1 || id <= 0){
        printf("invalid employee id.\n");
        clearBuffer();
        return;
    }
    clearBuffer();
    if(findEmp(id) != NULL){
        printf("employee already exists.\n");
        return;
    }
    if(scanf(" %99[^\n]",name) != 1){
        printf("invalid name.\n");
        clearBuffer();
        return;
    }
    clearBuffer();
    if(scanf(" %99[^\n]",designation) != 1){
        printf("invalid designation.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    if(ceo != NULL){
        printf("Enter reporting Manager ID: ");
        if(scanf("%d" , &managerID) != 1 || managerID <= 0){
            printf("invalid manager id.\n");
            clearBuffer();
            return;
        }
        clearBuffer();
        if(managerID == id){
            printf("invalid.\n");
            return;
        }
        manager = findEmp(managerID);
        if(manager == NULL){
            printf("no reporting found.\n");
            return;
        }
        if(manager->reportCount >= MAX_EMP){
            printf("invalid.\n");
            return;
        }
    }
    struct Employee *newEmployee = malloc(sizeof (struct Employee));
    if(newEmployee == NULL){
        printf("allocation failed.\n");
        return;
    }

    newEmployee->id = id;
    strcpy(newEmployee->name , name);
    strcpy(newEmployee->designation , designation);
    newEmployee->manager = manager;
    newEmployee->reportCount = 0;
    if(manager != NULL){
        manager->reports[manager->reportCount] = newEmployee;
        manager->reportCount++;
    }else{
        ceo = newEmployee;
    }
    employeeCount++;
    if(manager == NULL){
        printf("this employee is the top-level employee (CEO).\n");
    }
}

int countEmployees(struct Employee *employee){
    if(employee == NULL) return 0;
    int total = 0;
    for(int i = 0 ; i < employee -> reportCount ; i++){
        total += 1 + countEmployees(employee->reports[i]);
    }
    return total;
}

void displayHierarchy(struct Employee *employee , int level){
    if(employee == NULL) return;
    for (int i = 0; i < level; i++){
        printf("    ");
    }
    printf("%d - %s - %s\n" , employee->id , employee->name , employee -> designation);
    for(int i = 0 ; i < employee->reportCount ; i++){
        displayHierarchy(employee->reports[i],level + 1);
    }
}

void showEmployees(){
    int id;
    printf("Enter employee id: ");
    if(scanf("%d" , &id) != 1){
        printf("invalid input.\n");
        clearBuffer();
        return;
    }
    clearBuffer();
    struct Employee *employee = findEmp(id);
    if(employee == NULL){
        printf("employee not found.\n");
        return;
    }
    displayHierarchy(employee , 0);
    printf("Total Employees Under %d: %d\n",id , countEmployees(employee));
}

void showOrg(){
    if(ceo == NULL) return;
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displayHierarchy(ceo , 0);
    printf("Total Employees: %d\n", 1 + countEmployees(ceo));
}

void removeEmployee(){
    int id;
    printf("Enter Employee ID to remove: ");
    if(scanf("%d", &id) != 1){
        printf("Invalid input.\n");
        clearBuffer();
        return;
    }
    clearBuffer();
    struct Employee *employee = findEmp(id);
    if(employee == NULL)   return;
    if(employee->manager == NULL){
        printf("CEO cannot be removed.\n");
        return;
    }
    struct Employee *manager = employee->manager;
    struct Employee *target = manager;
    int fewest = -1;
    for(int i = 0; i < manager->reportCount; i++){
        struct Employee *sibling = manager->reports[i];
        if(sibling == employee)    continue;    
        if(fewest == -1 || sibling->reportCount < fewest) {
            fewest = sibling->reportCount;
            target = sibling;
        }
    }
    int targetCount = target->reportCount;
    if(target == manager) targetCount--;
    if(targetCount + employee->reportCount > MAX_EMP){
        printf("Cannot remove.\n");
        return;
    }
    int position = 0;
    while(manager->reports[position] != employee) position++;
    for(int i = position; i < manager->reportCount - 1; i++){
        manager->reports[i] = manager->reports[i + 1];
    }
    manager->reportCount--;
    for(int i = 0; i < employee->reportCount; i++){
        struct Employee *report = employee->reports[i];
        report->manager = target;
        target->reports[target->reportCount] = report;
        target->reportCount++;
    }
    free(employee);
    employeeCount--;
}

int main(){
    int choice , r;
    while(1){
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        r = scanf("%d", &choice);
        if(r == EOF){
            freeTree(ceo);
            return 0;
        }
        if(r != 1){
            printf("invalid input.\n");
            clearBuffer();
            return 0;
        }
        clearBuffer();
        switch (choice){
            case 1:
                addEmployee();
                break;
            case 2:
                removeEmployee();
                break;
            case 3:
                showEmployees();
                break;
            case 4:
                showOrg();
                break;
            case 5:
                freeTree(ceo);
                return 0;
            default:
                printf("invalid choice.\n");
        }
    }
    return 0;
}
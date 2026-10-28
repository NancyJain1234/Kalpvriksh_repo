/**
 * FILENAME: Hierarchy.c
 * DESCRIPTION: Develop a C program to manage an organization's employee hierarchy.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define NAME_SIZE 100
#define DEG_SIZE 50
#define LINE_LEN 200
#define INDENT 4
#define INPUT_EOF INT_MIN

struct Employee{
    int id;
    char name[NAME_SIZE];
    char designation[DEG_SIZE];
    struct Employee *manager;
    struct Employee *firstReport;
    struct Employee *nextReport;
};   

int readLine(char *buffer , int size){
    if(fgets(buffer , size , stdin) == NULL) return -1;
    char *newLine = strchr(buffer , '\n');
    if(newLine != NULL){
        *newLine = '\0';
        return 1;
    }
    if(feof(stdin)) return 1;
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF){}
    return 0;
}

int inputInt(const char *prompt){
    char input[LINE_LEN] , extra;
    int n;
    while(1){
        printf("%s" , prompt);
        int r = readLine(input , sizeof(input));
        if(r == -1) return INPUT_EOF;
        if(r == 1 && sscanf(input , "%d %c", &n , &extra) == 1 && n > 0)    return n;
        printf("Enter only an integer.\n");
    }
}

int inputText(const char *prompt , char *buffer , int size){
    char input[LINE_LEN];
    while(1){
        printf("%s", prompt);
        int r = readLine(input , sizeof(input));
        if(r == -1) return -1;
        if(r == 0 || strlen(input) >= size){
            printf("Text must be shorter.\n");
            continue;
        }
        int found = 0 , valid = 1;
        for(int i = 0 ; input[i] != '\0' ; i++){
            unsigned char c = (unsigned char)input[i];
            if(!isalpha(c) && !isspace(c)){
                valid = 0;
                break;
            }
            if(!isspace(c)) found = 1;
        }
        if(!valid){
            printf("Enter only letters.\n");
            continue;
        }
        if(!found){
            printf("Text can't be empty.\n");
            continue;
        }
        strcpy(buffer , input);
        return 1;
    }
}

struct Employee *searchEmployee(struct Employee *employee , int id){
    if(employee == NULL) return NULL;
    if(employee->id == id) return employee;
    struct Employee *found = searchEmployee(employee->firstReport , id);
    if(found != NULL) return found;
    return searchEmployee(employee->nextReport , id);
}

int countReports(struct Employee *employee){
    if(employee == NULL) return 0;
    return 1 + countReports(employee->nextReport);
}

int countEmployee(struct Employee *root){
    if(root == NULL) return 0;
    return 1 + countEmployee(root->firstReport) + countEmployee(root->nextReport);
}

struct Employee *firstLastEmployee(struct Employee *employee){
    if(employee == NULL || employee->nextReport==NULL) return employee;
    return firstLastEmployee(employee->nextReport);
}

struct Employee *firstBestManager(struct Employee *curr , struct Employee *target , struct Employee *best , int *minReports){
    if(curr == NULL) return best;
    if(curr != target){
        int count = countReports(curr -> firstReport);
        if(count < *minReports){
            *minReports = count;
            best = curr;
        }
    }
    return firstBestManager(curr->nextReport , target , best , minReports);
}

void changeManager(struct Employee *employee , struct Employee *manager){
    if(employee == NULL) return;
    employee->manager = manager;
    changeManager(employee->nextReport , manager);
}

void unlinkReport(struct Employee **head , struct Employee *target){
    if(*head == NULL)  return;
    if(*head == target){
        *head = target->nextReport;
        return;
    }
    unlinkReport(&((*head)->nextReport) , target);
}

void linkReports(struct Employee *manager , struct Employee *employee){
    employee->manager = manager;
    employee->nextReport = NULL;
    if(manager->firstReport == NULL) manager->firstReport = employee;
    else{
        firstLastEmployee(manager->firstReport)->nextReport = employee;
    }
}

void freeTree(struct Employee *employee){
    if(employee == NULL) return;
    freeTree(employee->firstReport);
    freeTree(employee->nextReport);
    free(employee);
}

struct Employee *createEmployee(int id){
    struct Employee *employee = malloc(sizeof(struct Employee));
    if(employee == NULL) return NULL;
    employee->id = id;
    employee->manager = NULL;
    employee -> firstReport = NULL;
    employee -> nextReport = NULL;
    return employee;
}

int readEmployeeDetails(struct Employee *employee){
    if(inputText("Enter employee name: ", employee->name , NAME_SIZE) == -1) return -1;
    if(inputText("Enter designation: ", employee -> designation , DEG_SIZE) == -1) return -1;
    return 1;
}

int readNewId(struct Employee *root){
    int id = inputInt("Enter employee ID: ");
    if(id == INPUT_EOF) return -1;
    if(searchEmployee(root , id) != NULL){
        printf("Employee ID already exists.\n");
        return 0;
    }
    return id;
}

int insertEmployee(struct Employee **root){
    int id = readNewId(*root);
    if(id <= 0) return id;
    struct Employee *newEmployee = createEmployee(id);
    if(newEmployee == NULL){
        printf("Memory allocation failed!\n");
        return -1;
    }
    if(readEmployeeDetails(newEmployee) == -1){
        free(newEmployee);
        return -1;
    }
    if(*root == NULL){
        *root = newEmployee;
        printf("CEO added successfully.\n");
        return 1;
    }
    int managerID = inputInt("Enter reporting manager ID: ");
    if(managerID == INPUT_EOF){
        free(newEmployee);
        return -1;
    }
    if(managerID == id){
        printf("An employee cannot manage themselves.\n");
        free(newEmployee);
        return 0;
    }
    struct Employee *manager = searchEmployee(*root , managerID);
    if(manager == NULL){
        printf("Manager not found!\n");
        free(newEmployee);
        return 0;
    }
    linkReports(manager , newEmployee);
    printf("Employee added successfully!\n");
    return 1;
}

void reassignReportees(struct Employee *reportees , struct Employee *best , struct Employee *manager){
    if(reportees == NULL) return;
    struct Employee *destination = (best != NULL) ? best : manager;
    changeManager(reportees , destination);
    if(destination->firstReport == NULL)    destination->firstReport = reportees;
    else firstLastEmployee(destination->firstReport)->nextReport = reportees;
}

int deleteEmployee(struct Employee **root , int id){
    struct Employee *target = searchEmployee(*root , id);
    if(target == NULL){
        printf("Employee does not exist!\n");
        return 0;
    }
    if(*root == target){
        printf("CEO can't be removed.\n");
        return 0;
    }
    struct Employee *manager = target->manager;
    struct Employee *reportees = target->firstReport;
    int minReports = INT_MAX;
    struct Employee *best = firstBestManager(manager->firstReport , target , NULL , &minReports);
    target->firstReport = NULL;
    unlinkReport(&manager->firstReport , target);
    reassignReportees(reportees , best , manager);
    free(target);
    printf("Employee removed successfully.\n");
    return 1;
}

void displayHierarchy(struct Employee *employee , int level , int showSibling){
    if(employee == NULL) return;
    printf("%*s%d - %s - %s\n" ,level * INDENT, "", employee->id , employee->name , employee -> designation);
    displayHierarchy(employee->firstReport , level + 1 , 1);
    if(showSibling) displayHierarchy(employee->nextReport , level , 1);
}

void showCompleteHierarchy(struct Employee *root){
    if(!root){
        printf("Organization is empty.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n\n");
    displayHierarchy(root , 0 , 0);
    printf("\nTotal Employees: %d\n", countEmployee(root));
}

int handleAdd(struct Employee **root){
    return insertEmployee(root);
}

int handleRemove(struct Employee **root){
    int id = inputInt("Enter Employee ID to remove: ");
    if(id == INPUT_EOF) return -1;
    deleteEmployee(root , id);
    return 0;
}

int handleShowOne(struct Employee *root){
    int id = inputInt("Enter Employee ID: ");
    if(id == INPUT_EOF) return -1;
    struct Employee *employee = searchEmployee(root , id);
    if(employee == NULL){
        printf("Employee not found.\n");
        return 0;
    }
    printf("\n");
    displayHierarchy(employee , 0 , 0);
    printf("\nTotal Employees Under %d: %d\n", id, countEmployee(employee->firstReport));
    return 0;
}

int main(){
    struct Employee *root = NULL;
    int choice;
    while(1){
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("\n");
        choice = inputInt("Enter your choice: ");
        if(choice == INPUT_EOF || choice == 5) break;

        switch (choice){
            case 1:
                if(handleAdd(&root) == -1) choice = 5;
                break;
            case 2:
                if(handleRemove(&root) == -1) choice = 5;
                break;
            case 3:
                if(handleShowOne(root) == -1) choice = 5;
                break;
            case 4:
                showCompleteHierarchy(root);
                break;
            default:
                printf("Invalid choice.\n");
        }
        if(choice == 5) break;
    }
    freeTree(root);
    return 0;
}
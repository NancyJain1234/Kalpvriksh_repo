/**
 * Filename : Assignment1(b)
 * Description : CRUD Operations in File
 * Author : Nancy Jain
 */

#include <stdio.h>
#include <string.h>

#define FILENAME "users.txt"

struct User{
    int id;
    char name[50];
    int age;
};

int idExists(int id){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if(file == NULL) {
        return 0; 
    }
    while(fscanf(file, "%d\n", &user.id) == 1){
        fgets(user.name, 50, file);
        user.name[strlen(user.name) - 1] = '\0';
        fscanf(file, "%d\n", &user.age);
        if(user.id == id) {
            fclose(file);
            return 1; 
        }
    }
    fclose(file);
    return 0; 
}

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

void getName(char message[], char name[]){
    while(1){
        printf("%s", message);
        fgets(name, 50, stdin);
        if(name[0] != '\n'){
            name[strlen(name) - 1] = '\0';
            return;
        }
        printf("Name cannot be empty.\n");
    }
}

void createUser(struct User user){
    FILE *file = fopen(FILENAME, "a");
    if(file == NULL){
        printf("Error opening file.\n");
        return;
    }   
    fprintf(file, "%d\n%s\n%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User created successfully.\n");
}

void readUsers(){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if(file == NULL){
        FILE *file = fopen(FILENAME, "a");
        if(file == NULL){
            printf("Error creating file.\n");
            return;
        }
    }
    printf("User List:\n");
    while(fscanf(file, "%d\n", &user.id) == 1){
        fgets(user.name, 50, file);
        user.name[strlen(user.name) - 1] = '\0';
        fscanf(file, "%d\n", &user.age);
        printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
    }
    fclose(file);
}

void updateUser(int id){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if(file == NULL || tempFile == NULL){
        printf("Error opening file.\n");
        return;
    }
    int found = 0;
    while(fscanf(file, "%d\n", &user.id) == 1){
        fgets(user.name, 50, file);
        user.name[strlen(user.name) - 1] = '\0';
        fscanf(file, "%d\n", &user.age);
        if (user.id == id){
            found = 1;
            getName("Enter new name: ", user.name);
            user.age = getInt("Enter new age: ");
        }
        fprintf(tempFile, "%d\n%s\n%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(tempFile);
    remove(FILENAME);
    rename("temp.txt", FILENAME);
    if (found){
        printf("User updated successfully.\n");
    } else {
        printf("User not found.\n");
    }
}

void deleteUser(int id){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if(file == NULL || tempFile == NULL){
        printf("Error opening file.\n");
        return;
    }
    int found = 0;
    while(fscanf(file, "%d\n", &user.id) == 1){
        fgets(user.name, 50, file);
        user.name[strlen(user.name) - 1] = '\0';
        fscanf(file, "%d\n", &user.age);
        if(user.id == id){
            found = 1;
            continue; 
        }
        fprintf(tempFile, "%d\n%s\n%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(tempFile);
    remove(FILENAME);
    rename("temp.txt", FILENAME);
    if (found){
        printf("User deleted successfully.\n");
    } else {
        printf("User not found.\n");
    }
}

int main(){
    int choice;
    do{
        printf("\nUser Management System\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        choice = getInt("Enter your choice: ");
        switch(choice){
            case 1:
            {
                struct User user;
                user.id = getInt("Enter user ID: ");
                if(idExists(user.id)){
                    printf("Error: User ID already exists.\n");
                    break;
                }
                getName("Enter user Name: ", user.name);
                user.age = getInt("Enter user Age: ");
                createUser(user);
                break;
            }
            case 2:
            {
                readUsers();
                break;
            }   
            case 3:
            {
                int id;
                id = getInt("Enter user ID to update: ");
                if(!idExists(id)){
                    printf("Error: User ID does not exist.\n");
                    break;
                }
                updateUser(id);
                break;
                }
            case 4:
            {   
                int id;
                id = getInt("Enter user ID to delete: ");
                deleteUser(id);
                break;
            }
            case 5:
                printf("Exiting!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while(choice != 5);
    return 0;
}
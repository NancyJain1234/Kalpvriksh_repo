/**
 * Filename : Assignment1(b)
 * Description : CRUD Operations in File
 * Author : Nancy Jain
 */

#include <stdio.h>
#include <string.h>

#define FILENAME "users.txt"
#define TEMPFILE "temp.txt"

#define NAME_LEN 50
#define MAX_ID 1000
#define MAX_AGE 150

struct User{
    int id;
    char name[NAME_LEN];
    int age;
};

void createFile(){
    FILE *file = fopen(FILENAME , "a");
    if(file == NULL){
        printf("Error Creating file.\n");
        return;
    }
    fclose(file);
}

int readUser(FILE *file , struct User *user){
    if(fscanf(file, "%d\n", &user->id) != 1){
        return 0;
    }
    if(fgets(user->name, NAME_LEN, file) == NULL){
        return 0;
    }
    user->name[strcspn(user->name, "\n")] = '\0';
    if(fscanf(file, "%d\n", &user->age) != 1){
        return 0;
    }
    return 1;
}

void writeUser(FILE *file , struct User user){
    fprintf(file, "%d\n%s\n%d\n", user.id , user.name , user.age);
}

int idExists(int id){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if(file == NULL) {
        return 0; 
    }
    while(readUser(file, &user)){  
        if(user.id == id) {
            fclose(file);
            return 1; 
        }
    }
    fclose(file);
    return 0; 
}

int getInt(char message[] , int min , int max){
    int num;
    int ch = 0;
    while(1){
        printf("%s", message);
        if(scanf("%d%c", &num, &ch) == 2 && ch == '\n' && num >= min && num <= max){
            return num;
        }
        printf("Invalid input. Enter a number between %d and %d.\n" , min , max);
        while(ch != '\n' && ch != EOF){
            ch = getchar();
        }
        ch = 0;
    }
}

void getName(char message[], char name[]){
    int ch;
    while(1){
        printf("%s", message);
        if(fgets(name, NAME_LEN, stdin) == NULL){
            name[0] = '\0';
            return;
        }
        if( strchr(name, '\n') == NULL){
            while((ch = getchar()) != '\n' && ch != EOF);
        }
        name[strcspn(name, "\n")] = '\0';
        if(name[0] != '\0'){
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
    writeUser(file , user);
    fclose(file);
    printf("User created successfully.\n");
}

void readUsers(){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if(file == NULL){
        printf("Error opening file.\n");
        return;
    }
    int count = 0;
    while(readUser(file , &user)){
        printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
        count++;
    }
    if(count == 0){
        printf("No users found.\n");
    }
    fclose(file);
}

void updateUser(int id , struct User newData){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    FILE *tempFile = fopen(TEMPFILE, "w");
    if(file == NULL || tempFile == NULL){
        printf("Error opening file.\n");
        if(file != NULL) fclose(file);
        if(tempFile != NULL) fclose(tempFile);
        return;
    }
    int found = 0;
    while(readUser(file , &user)){
        if (user.id == id){
            found = 1;
            strcpy(user.name , newData.name);
            user.age = newData.age;
        }
        writeUser(tempFile , user);
    }
    fclose(file);
    fclose(tempFile);
    if(remove(FILENAME) != 0 || rename(TEMPFILE , FILENAME) != 0){
        printf("Error Updating File\n");
        return;
    }
    if (found){
        printf("User updated successfully.\n");
    } else {
        printf("User not found.\n");
    }
}

void deleteUser(int id){
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    FILE *tempFile = fopen(TEMPFILE, "w");
    if(file == NULL || tempFile == NULL){
        printf("Error opening file.\n");
        if(file != NULL) fclose(file);
        if(tempFile != NULL) fclose(tempFile);
        return;
    }
    int found = 0;
    while(readUser(file , &user)){
        if(user.id == id){
            found = 1;
            continue; 
        }
        writeUser(tempFile , user);
    }
    fclose(file);
    fclose(tempFile);
    if(remove(FILENAME) != 0 || rename(TEMPFILE, FILENAME) != 0){
        printf("Error deleting from file.\n");
        return;
    }
    if (found){
        printf("User deleted successfully.\n");
    } else {
        printf("User not found.\n");
    }
}

int main(){
    createFile();
    int choice;
    do{
        printf("\nUser Management System\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        choice = getInt("Enter your choice: " , 1 , 5);
        switch(choice){
            case 1:
            {
                struct User user;
                user.id = getInt("Enter user ID: " , 1, MAX_ID);
                if(idExists(user.id)){
                    printf("Error: User ID already exists.\n");
                    break;
                }
                getName("Enter user Name: ", user.name);
                user.age = getInt("Enter user Age: " , 1 , MAX_AGE);
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
                struct User newData;
                id = getInt("Enter user ID to update: " , 1 , MAX_ID);
                if(!idExists(id)){
                    printf("Error: User ID does not exist.\n");
                    break;
                }
                getName("Enter new Name: " , newData.name);
                newData.age = getInt("Enter new age: ", 1, MAX_AGE);
                updateUser(id , newData);
                break;
            }
            case 4:
            {   
                int id;
                id = getInt("Enter user ID to delete: " , 1 , MAX_ID);
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
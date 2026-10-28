/**
 * FILENAME: SolarImageProcessing.c
 * DESCRIPTION: Analyzing sonar images to track whale movements.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PIXELS 256

void freeMatrix(int n,int **matrix){
    if(matrix == NULL) return;
    for(int i = 0 ; i < n ; i++){
        free(*(matrix + i));
    }
    free(matrix);
}

void RotateP(int n , int **matrix){
    int temp , *p , *q;
    for(int i = 0 ; i < n ; i++){               //transpose
        for(int j = i + 1 ; j < n ; j++){
            p = *(matrix + i) + j;
            q = *(matrix + j) + i;
            temp = *p;
            *p = *q;
            *q = temp;
        }
    }
    for(int i = 0 ; i < n ; i++){               //reverse
        for(int j = 0 ; j < n / 2 ; j++){
            p = *(matrix + i) + j;
            q = *(matrix + i) + (n - j - 1);
            temp = *p;
            *p = *q;
            *q = temp;
        }
    }
}

void smoothingFilter(int n , int **matrix){
    int **result = malloc(n * sizeof(int *));
    if(result == NULL){
        printf("Dynamic Allocation failed!");
        return;
    }

    for(int i = 0 ; i < n ; i++){
        *(result + i) = malloc(n * sizeof(int));
        if (*(result + i) == NULL){
                printf("Dynamic allocation failed!\n");
                for (int j = 0; j < i; j++) free(*(result + j));
                free(result);
                return;
            }
        }

    for(int i = 0 ; i < n ; i++){    
        for(int j = 0 ; j < n ; j++){
            int count = 0 , sum = 0;

            for(int k = -1 ; k <= 1 ; k++){
                for(int l = -1 ; l <= 1 ; l++){
                    int r = i + k , c = j + l;
                    if(r >= 0 && r < n && c >= 0 && c < n){
                        sum += *(*(matrix + r) + c);
                        count++;
                    }
                }
            }
            *(*(result + i) + j) = sum / count;
        }
    }
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            *(*(matrix + i) + j) = *(*(result + i) + j);
        }
    }
    freeMatrix(n , result);
}

void generateImage(int n , int **matrix){
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            *(*(matrix + i) + j) = rand() % PIXELS;
        }
    }
}

void displayMatrix(int n , int **matrix){
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            printf("%4d", *(*(matrix + i) + j));
        }
        printf("\n");
    }
}

int main(){
    int n;
    printf("Enter matrix size (2-10): \n");
    if(scanf("%d" , &n) != 1 || n < 2 || n > 10){
        printf("Invalid input.");
        return 1;
    }
    int **matrix = malloc(n * sizeof(int *));
    if(matrix == NULL){
        printf("Dynamic Allocation failed!\n");
        return 1;
    }
    for(int i = 0 ; i < n ; i++){
        *(matrix + i) = malloc(n * sizeof(int));
        if(*(matrix + i) == NULL){
            printf("Memory Allocation failed!");
            freeMatrix(i , matrix);
            return 1;
        }
    }
    srand((unsigned int)time(NULL));
    generateImage(n , matrix);
    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(n , matrix);

    RotateP(n , matrix);
    printf("\nMatrix after 90%c Clockwise Rotation:\n", 176);
    displayMatrix(n , matrix);

    smoothingFilter(n , matrix);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(n , matrix);
    freeMatrix(n , matrix);
    return 0;
}




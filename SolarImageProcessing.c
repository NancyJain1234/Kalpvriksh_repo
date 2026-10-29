/**
 * FILENAME: SolarImageProcessing.c
 * DESCRIPTION: Analyzing sonar images to track whale movements.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PIXELS 256

void RotateP(int n , int **matrix){
    for(int i = 0 ; i < n ; i++){               //transpose
        for(int j = i + 1; j < n ; j++){
            int **p = (matrix + i * n + j);
            int **q = (matrix + j * n + i);
            int *temp = *p;
            *p = *q;
            *q = temp;
        }
    }
    for(int i = 0 ; i < n ; i++){               //reverse
        for(int j = 0 ; j < n / 2 ; j++){
            int **p = (matrix + i * n + j);
            int **q = (matrix + i * n + (n - 1 + j));
            int *temp = *p;
            *p = *q;
            *q = temp;
        }
    }
}

int smoothingFilter(int n , int **matrix){
    int *prev = malloc(n * sizeof(int));
    int *curr = malloc(n * sizeof(int));
    if(prev == NULL || curr == NULL){
        printf("Dynamic Allocation failed!");
        free(prev);
        free(curr);
        return 0;
    }

    for(int i = 0 ; i < n ; i++){   
        for(int m = 0 ; m < n ; m++){
            *(curr + m) = **(matrix + i * n + m);
        } 
        for(int j = 0 ; j < n ; j++){
            int count = 0 , sum = 0;
            for(int k = -1 ; k <= 1 ; k++){
                for(int l = -1 ; l <= 1 ; l++){
                    int r = i + k , c = j + l;
                    if(r < 0 || r >= n || c < 0 || c >= n) continue;
                    if(k == -1) sum += *(prev + c);
                    else if(k == 0) sum += *(curr + c);
                    else sum += **(matrix + r * n + c);
                    count++;
                }
            }
            **(matrix + i * n + j) = sum / count;
        }
        int *t = prev;
        prev = curr;
        curr = t;
    }
    free(prev);
    free(curr);
    return 1;
}

void generateImage(int n , int *data){
    for(int i = 0 ; i < n ; i++){
        *(data + i) = rand() % PIXELS;
    }
}

void displayMatrix(int n , int **matrix){
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            printf("%4d", **(matrix + i * n + j));
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
    int *data = malloc(n * n * sizeof(int));
    int **matrix = malloc(n * n * sizeof(int *));
    if(data == NULL || matrix == NULL){
        printf("Dynamic Allocation failed!\n");
        free(data);
        free(matrix);
        return 1;
    }

    srand((unsigned int)time(NULL));
    generateImage(n * n , data);
    for(int i = 0 ; i < n * n; i++){
        *(matrix + i) = data + i;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(n , matrix);

    RotateP(n , matrix);
    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    displayMatrix(n , matrix);

    if(!smoothingFilter(n , matrix)){
        printf("Dynamic Allocation failed!\n");
        free(data);
        free(matrix);
        return 1;
    }
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(n , matrix);
    free(data);
    free(matrix);
    return 0;
}




/**
 * Filename: StudentNameEncodeScoreAnalyzer.c
 * Description: Student Name Encoder + Score Matrix Analyzer
 * Author: Nancy Jain
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>

 int isVowel(char ch){
    ch = tolower((unsigned char)ch);
    return ch == 'a' || ch == 'e' || ch == 'i' || ch =='o' || ch == 'u';
 }

 char *encodeName(char *input){
    int len = strlen(input);
    char *output = malloc(len + 1);
    if(output == NULL) return NULL;
    int j = 0;
    for(int i = 0 ; input[i] != '\0' ; i++){
        char c = input[i];
        if(i > 0 && c == input[i - 1]){
            continue;
        }
        if(isVowel(c) && j > 0 && isVowel(output[j - 1])){
            continue;
        }
        output[j++] = tolower((unsigned char)c);
    }
    output[j] = '\0';
    return output;
 }

 int** allocateMatrix(int n){
    int **marks = malloc(n * sizeof(int *));
    if(marks == NULL) return NULL;
    for(int i = 0 ; i <  n ; i++){
        marks[i] = malloc( 5 * sizeof(int));
        if(marks[i] == NULL){
            for(int k = 0 ; k < i ; k++){
                free(marks[k]);
            }
            free(marks);
            return NULL;
        }
    }
    return marks;
 }

int computeTotal(int *row){
    int total = 0;
    for(int j = 0 ; j < 5 ; j++){
        total += row[j];
    }
    return total;
}

double computeAvg(int *row){
    return computeTotal(row) / 5.0;
}

int highestSubject(int *row){
    int maxIndex = 0;
    for(int j = 1 ; j < 5 ; j++){
        if(row[j] > row[maxIndex]){
            maxIndex = j;
        }
    }
    return maxIndex;
}

void printResult(char **names , int **marks , int n){
    for(int i = 0 ; i < n ; i++){
        printf("%s %d %.2f %d\n" , names[i] , computeTotal(marks[i]) , computeAvg(marks[i]) , highestSubject(marks[i]));
    }
}

int main(){
    int n ;
    if(scanf("%d", &n) != 1 || n <= 0)  return 1;
    char **names = malloc(n *sizeof(char *));
    int **marks = allocateMatrix(n);
    if(names == NULL || marks == NULL){
        free(names);
        if(marks != NULL){
            for(int i = 0 ; i < n ; i++){
                free(marks[i]);
            }
            free(marks);
        }
        return 1;
    }
    
    for(int i = 0 ; i < n ; i++){
        char input[1000];
        scanf("%999s" , input);
        names[i] = encodeName(input);
        if(names[i] == NULL){
            for(int k = 0 ; k < i ; k++){
                free(names[k]);
            }
            free(names);
            for(int k = 0 ; k < n ; k++){
                free(marks[k]);
            }
            free(marks);
            return 1;
        }
        for(int j = 0 ; j < 5 ; j++){
            scanf("%d" , &marks[i][j]);
        }
    }
    printResult(names , marks , n);
    for(int i = 0 ; i < n ; i++){
        free(names[i]);
        free(marks[i]);
    }
    free(names);
    free(marks);
    return 0;
}
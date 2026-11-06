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

#define SIZE 10

int getDegree(double c[]){
    for(int i = SIZE - 1 ; i >= 0 ; i--){
        if(c[i] != 0) return i;
    }
    return 0;
}

double valueAt(double c[] , double x){
    double answer = 0;
    for(int i = getDegree(c) ; i >= 0 ; i--){
        answer = answer * x + c[i];
    }
    return answer;
}

int parse(char s[], int c[]){
    int i = 0 , p , termCount = 0;
    for(p = 0 ; p < SIZE ; p++) c[p] = 0;
    while(1){
        int sign = 1;
        while(s[i] == ' ')    i++;
        if(s[i] == '\0') break;
        int begin = i;
        if(s[i] == '+') i++;
        else if(s[i] == '-'){
            sign = -1;
            i++;
            while(s[i] == ' ')    i++;
        }else if(termCount > 0) return 0;
        int num = 0 , foundNum = 0;
        if(isdigit(s[i])){
            num = 0;
            while(isdigit(s[i])){
                num = num * 10 + s[i] - '0';
                foundNum = 1;
                if(num > 1000000) return 0;
                i++;
            }
        }
        int power = 0 , foundX = 0;
        while(s[i] == ' '){
            i++;
        }
        if(s[i] == 'x' || s[i] == 'X'){
            foundX = 1;
            if(!foundNum) num = 1;
            power = 1;
            i++;
            if(s[i] == '^'){
                i++;
                if(!isdigit(s[i])) return 0;
                power = 0;
                while(isdigit(s[i])){
                    power = power * 10 + (s[i] - '0');
                    if(power > SIZE) return 0;
                    i++;
                }
            }
        }
        if(!foundNum && !foundX) return 0;
        c[power] += sign * num;
        termCount++;
    }
    if(termCount == 0) return 0;
    return 1;
}

void show(int c[]){
    int first = 1;
    for(int i = getDegree(c) ; i >= 0 ; i--){
        if(c[i] == 0) continue;
        if(first){
            if(c[i] < 0) printf(" - ");
        }else{
            if(c[i] < 0) printf(" - ");
            else printf(" + ");
        }
        if(fabs(c[i]) != 1 || i == 0){
            printf("%g", fabs(c[i]));
        }
        if(i >= 1) printf("x");
        if(i >= 2) printf("^%d" , i);
        first = 0;
    }
    if(first) printf("0");
}

void add(double A[] , double B[] , double C[]){
    for(int i = 0 ; i < SIZE ; i++){
        C[i] = A[i] + B[i];
    }
}

void subtract(double A[] , double B[] , double C[]){
    for(int i = 0 ; i < SIZE ; i++){
        C[i] = A[i] - B[i];
    }
}

int multiply(double A[] , double B[] , double C[]){
    if(getDegree(A) + getDegree(B) >= SIZE){
        printf("Error: Overflow.\n");
        return 0;
    }
    for( int i = 0 ; i < SIZE ; i++) C[i] = 0;
    for(int i = 0 ; i <= getDegree(A) ; i++){
        for(int j = 0 ; j <= getDegree(B) ; j++){
            C[i + j] += A[i] * B[j];
        }
    }
    return 1;
}

int divide(double A[] , double B[] , double q[] , double rem[]){
    int degA = getDegree(A) , degB = getDegree(B);
    if(degB == 0 && B[0] == 0){
        printf("Error: cannot divide by 0");
        return 0;
    }
    for(int i = 0 ; i < SIZE ; i++){
        q[i] = 0;
        rem[i] = A[i];
    }
    for(int i = degA - degB ; i >= 0 ; i--){
        q[i] = rem[i + degB] / B[degB];
        for(int j = 0 ; j <= degB ; j++){
            rem[i + j] -= q[i] * B[j];
        }
    }
    for(int i = 0 ;i < SIZE ; i++){
        if(fabs(rem[i]) < 0.000001) rem[i] = 0;
    }
    return 1;
}

void showRoot(double r){
    if(fabs(r) < 0.000001)  printf("x");
    else if(r > 0)          printf("(x - %.4g)" , r);
    else                    printf("(x + %.4g)" , -r);
}

void showFactors(double roots[] , int count , double leftover[]){
    int leftoverDegree = getDegree(leftover);
    if(leftoverDegree == 0 && leftover[0] != 1){
        printf("%g" , leftover[0]);
    }
    for(int i = 0 ; i < count ; i++){
        showRoot(roots[i]);
    }
    if(leftoverDegree > 0){
        printf("(");
        show(leftover);
        printf(")");
        if(leftoverDegree == 2){
            printf("  <- no real factors (imaginary roots)");
        }
    }
    printf("\n");
}

int getFactors(double c[] , double roots[] , double leftover[]){
    double nw[SIZE];
    int count = 0;
    int degree = getDegree(c);
    for(int i = 0 ; i < SIZE ; i++) leftover[i] = c[i];
    for(int r = -50 ; r <= 50 ; r++){
        while(degree >= 1 && fabs(valueAt(leftover , r)) < 0.000001){
            roots[count++] = r;
            for(int i = 0 ; i < SIZE ; i++) nw[i] = 0;
            nw[degree - 1] = leftover[degree];
            for(int i = degree - 1 ; i >= 1 ; i--){
                nw[i - 1] = leftover[i] + r * nw[i];
            }
            for(int i = 0 ; i < SIZE ; i++) leftover[i] = nw[i];
            degree--;
        }
    }
    if(degree == 2){
        double a = leftover[2] , b = leftover[1] , k = leftover[0];
        double d = b * b - 4 * a * k;
        if(d >= 0){
            roots[count++] = (-b + sqrt(d)) / (2 * a);
            roots[count++] = (-b - sqrt(d)) / (2 * a);
            for(int i = 0 ; i < SIZE ; i++) leftover[i] = 0;
            leftover[0] = a;
        }
    }
    return count;
}

int askPolynomial(char question[] , double c[]){
    char s[200];
    while(1){
        printf("%s" , question);
        if(fgets(s , sizeof(s) , stdin) == NULL) return 0;
        s[strcspn(s , "\n")] = '\0';
        if(parse(s , c)) return 1;
        printf("Invalid polynomial. Use a form like 5x^2 + 10x - 6\n");
    }
}

int main(){
    double A[SIZE] , B[SIZE];
    double c[SIZE] , q[SIZE] , rem[SIZE];
    double roots[SIZE] , leftover[SIZE];
    char line[100];
    int choice , count;

    printf("Enter polynomials in the form: 5x^2 + 10x - 6\n\n");
    if(!askPolynomial("Enter first polynomial  : " , A)) return 1;
    if(!askPolynomial("Enter second polynomial : " , B)) return 1;
    printf("\nFirst  polynomial = ");
    show(A);
    printf("\nSecond polynomial = ");
    show(B);
    printf("\n");

    do{
        printf("\n------ MENU ------\n");
        printf("1. Addition\n");
        printf("2. Subtraction\n");
        printf("3. Division\n");
        printf("4. Multiplication\n");
        printf("5. Finding factors\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        if(fgets(line , sizeof(line) , stdin) == NULL) break;
        if(sscanf(line , "%d" , &choice) != 1) choice = 0;
        printf("\n");
        switch(choice){
            case 1:
                add(A , B , c);
                printf("Sum        = ");
                show(c);
                printf("\n");
                break;
            case 2:
                subtract(A , B , c);
                printf("Difference = ");
                show(c);
                printf("\n");
                break;
            case 3:
                if(!divide(A , B , q , rem)){
                    printf("Error: cannot divide by 0.\n");
                }else{
                    printf("Quotient   = ");
                    show(q);
                    printf("\nRemainder  = ");
                    show(rem);
                    printf("\n");
                }
                break;
            case 4:
                if(!multiply(A , B , c)){
                    printf("Error: product is too big (power 20 or more).\n");
                }else{
                    printf("Product    = ");
                    show(c);
                    printf("\n");
                }
                break;
            case 5:
                count = getFactors(A , roots , leftover);
                printf("Factors of first polynomial  = ");
                showFactors(roots , count , leftover);
 
                count = getFactors(B , roots , leftover);
                printf("Factors of second polynomial = ");
                showFactors(roots , count , leftover);
                break;
            case 6:
                printf("Bye!\n");
                break;
            default:
                printf("Error: invalid choice, enter 1 to 6.\n");
        }
    }while(choice != 6);
    return 0;
}

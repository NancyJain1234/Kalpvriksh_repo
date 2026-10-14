/**
 * FILENAME : Nancy Jain 
 * Description : console-based program in C for a small cinema ticket counter.
 * Author - Nancy Jain
*/

#include <stdio.h>
#include<string.h>

#define MAX_TICKETS 20
#define MIN_DISCOUNT 5
#define MIN_TICKETS 1
#define NAME_SIZE 50

const float STANDARD = 150.0f;
const float PREMIUM = 250.0f;
const float RECLINER = 400.0f;
const float DISCOUNT_RATE = 0.10f;

struct Booking {
    char name[NAME_SIZE];
    int tickets;
    int choice;
    const char *category;
    float price;
    float subtotal;
    float discount;
    float finalBill;
};

int isValid(int tickets){
    return (tickets >= MIN_TICKETS && tickets <= MAX_TICKETS);
}

int selectCategory(int choice , struct Booking *booking){
    switch(choice) {
        case 1:
            booking -> category = "Standard";
            booking -> price = STANDARD;
            return 1;
        
        case 2:
            booking -> category = "Premium";
            booking -> price = PREMIUM;
            return 1;

        case 3:
            booking -> category = "Recliner";
            booking -> price = RECLINER;
            return 1;
        
        default:
            return 0;
    }
}

float calculateSubtotal(int tickets , float price){
    return tickets * price;
}

float calculateDiscount(int tickets , float subtotal){
    if(tickets >= MIN_DISCOUNT){
        return subtotal * DISCOUNT_RATE;
    }
    return 0.0f;
}

float calculateFinalBill(float subtotal , float discount){
    return subtotal - discount;
}

int processBooking(struct Booking *booking){
    if(!isValid(booking -> tickets)){
        return 1;
    }

    if(!selectCategory(booking -> choice , booking)){
        return 2;
    }

    booking -> subtotal = calculateSubtotal(booking -> tickets , booking -> price);
    booking -> discount = calculateDiscount(booking -> tickets , booking -> subtotal);
    booking -> finalBill = calculateFinalBill(booking -> subtotal , booking -> discount);
    return 0;
}

 int main(){
    struct Booking booking;
    int status , ch;
    if(fgets(booking.name, sizeof(booking.name) , stdin) == NULL){
        printf("Invalid input.\n");
        return 0;
    }
    booking.name[strcspn(booking.name, "\n")] = '\0';
    if(booking.name[0] == '\0'){
        printf("Invalid input.\n");
        return 0;
    }
    if(scanf("%d %d", &booking.tickets, &booking.choice) != 2){
        printf("Error: Invalid input.\n");
        return 0;
    }
    status = processBooking(&booking);

    if(status == 1){
        printf("Error : Invalid ticket count.\n");
    } else if(status == 2){
        printf("Invalid ticket category.\n");
    } else {
        printf("Customer : %s \n" , booking.name);
        printf("Ticket Category: %s\n" , booking.category);
        printf("Tickets: %d\n", booking.tickets);
        printf("Subtotal: %.2f\n", booking.subtotal);
        printf("Discount: %.2f\n" , booking.discount);
        printf("Final Bill: %.2f\n",booking.finalBill);
    }
    return 0;
 }
/**
 * Filename: Assignment2.c
 * Description: Console-based program in C for a small cinema ticket counter.
 * Author: Nancy Jain
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TICKETS 20
#define MIN_DISCOUNT 5
#define MIN_TICKETS 1
#define NAME_SIZE 100

const double STANDARD = 150.0;
const double PREMIUM = 250.0;
const double RECLINER = 400.0;
const double DISCOUNT_RATE = 0.10;

enum BookingStatus {
    BOOKING_OK,
    ERR_TICKETS,
    ERR_CATEGORY
};

struct Booking {
    char name[NAME_SIZE];
    int tickets;
    int choice;
    const char *category;
    double price;
    double subtotal;
    double discount;
    double finalBill;
};

int isValid(int tickets){
    return (tickets >= MIN_TICKETS && tickets <= MAX_TICKETS);
}

int selectCategory(int choice , struct Booking *booking){
    switch(choice) {
        case 1:
            booking->category = "Standard";
            booking->price = STANDARD;
            return 1;
        
        case 2:
            booking->category = "Premium";
            booking->price = PREMIUM;
            return 1;

        case 3:
            booking->category = "Recliner";
            booking->price = RECLINER;
            return 1;
        
        default:
            return 0;
    }
}

double calculateSubtotal(int tickets , double price){
    return tickets * price;
}

double calculateDiscount(int tickets , double subtotal){
    if(tickets >= MIN_DISCOUNT){
        return subtotal * DISCOUNT_RATE;
    }
    return 0.0;
}

double calculateFinalBill(double subtotal , double discount){
    return subtotal - discount;
}

enum BookingStatus processBooking(struct Booking*booking){
    if(!isValid(booking->tickets)){
        return ERR_TICKETS;
    }

    if(!selectCategory(booking->choice , booking)){
        return ERR_CATEGORY;
    }

    booking->subtotal = calculateSubtotal(booking->tickets , booking->price);
    booking->discount = calculateDiscount(booking->tickets , booking->subtotal);
    booking->finalBill = calculateFinalBill(booking->subtotal , booking->discount);
    return BOOKING_OK;
}

int main(void){
    struct Booking booking;
    enum BookingStatus status;

    int ch;
    if(fgets(booking.name, sizeof(booking.name) , stdin) == NULL){
        printf("Error: Invalid input.\n");
        return 1;
    }

    char *newline = strchr(booking.name, '\n');
    if(newline != NULL) {
        *newline = '\0';
    }else{
        ch = getchar();
        if(ch != '\n' && ch != EOF){
            while(ch != '\n' && ch != EOF){
                ch = getchar();
            }
            printf("Error: Invalid input.\n");
            return 1;
        }
    }

    int i = 0 , validName = 0;
    while(booking.name[i] != '\0'){
        if(!isspace((unsigned char)booking.name[i])){
            validName = 1;
            break;
        }
        i++;
    }
    if(!validName){
        printf("Error: Invalid input.\n");
        return 1;
    }
    if(scanf("%d %d", &booking.tickets, &booking.choice) != 2){
        printf("Error: Invalid input.\n");
        return 1;
    }
    status = processBooking(&booking);

    if(status == ERR_TICKETS){
        printf("Error: Invalid ticket count.\n");
        return 1;
    }
    if(status == ERR_CATEGORY){
        printf("Error: Invalid ticket category.\n");
        return 1;
    }
    if(status != BOOKING_OK){
        return 1;
    }
    printf("Customer: %s\n" , booking.name);
    printf("Ticket Category: %s\n" , booking.category);
    printf("Tickets: %d\n", booking.tickets);
    printf("Subtotal: %.2f\n", booking.subtotal);
    printf("Discount: %.2f\n" , booking.discount);
    printf("Final Bill: %.2f\n",booking.finalBill);

    return 0;
}
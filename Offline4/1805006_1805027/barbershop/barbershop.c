#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include "../zemaphore/zemaphore.h"
// #include "../zemaphore/zemaphore.c"
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>


void cut_hair();
void *make_customer();

pthread_mutex_t main_chair_lock;
zem_t zem_barber;
zem_t zem_customer;
zem_t zem_waiting_seats;
zem_t zem_customer_done;
zem_t zem_barber_done;


int total_waiting_seats;
int total_customers;
int free_seats;
int left_customers = 0;

void *f_barber(void *ptr){
    int haircut_count = 0;
    while(1)
    {
        zem_down(&zem_customer);    //wait for customer
                
        zem_down(&zem_waiting_seats);

        free_seats++;
        printf("2. Customer Called\n");
        printf("Free Seats: %d\n", free_seats);

        zem_up(&zem_waiting_seats);
        zem_up(&zem_barber);        //tell customers barber ready
        pthread_mutex_lock(&main_chair_lock);
        //give_haircut();
        printf("3a. Cutting Hair\n");

        pthread_mutex_unlock(&main_chair_lock);

        
        haircut_count++;
        zem_down(&zem_customer_done);
        printf("4. Cut Done\n");
        zem_up(&zem_barber_done);

        if(haircut_count + left_customers == total_customers) break;

    }
    return 0;
}

void *f_customer(void *ptr){
    int id = *(int *) ptr;
    zem_down(&zem_waiting_seats);

    if(free_seats >= 1){
        free_seats--;
        printf("1. Customer[id:%d] is waiting\n", id);
        printf("Free Seats: %d seats\n", free_seats);

        zem_up(&zem_customer);
        zem_up(&zem_waiting_seats);

        zem_down(&zem_barber);

        printf("3b. Customer[id:%d]'s hair is being cut\n", id);

        zem_up(&zem_customer_done);
        zem_down(&zem_barber_done);





    }
    else{
        left_customers++;
        printf("--- Customer[id:%d] left\n", id);
        zem_up(&zem_waiting_seats);

    }
    return 0;
}


int main(int argc, char *argv[]){
    pthread_t barber_t;
    pthread_t *customers_t;
    int * customer_id;
    if (argc < 3) {
        printf("./barbershop #total_chairs #total_customers.g. ./exe 15 25\n");
        exit(1);
    }

    pthread_mutex_init(&main_chair_lock, NULL);
    zem_init(&zem_customer, 0);
    zem_init(&zem_barber, 0);
    zem_init(&zem_waiting_seats, 1);
    zem_init(&zem_customer_done, 0);
    zem_init(&zem_barber_done, 0);

    free_seats = atoi(argv[1]);
    total_customers = atoi(argv[2]);

    pthread_create(&barber_t, NULL, (void *)f_barber, NULL );
    printf("0.0. Barber Created\n");

    customer_id = (int *)malloc(sizeof(int) * total_customers);

    for (int i = 0; i < total_customers; i++)
        customer_id[i] = i;

    customers_t = (pthread_t *)malloc(sizeof(pthread_t) * total_customers);
    
    for (int i = 0; i < total_customers; i++){
        pthread_create(&customers_t[i], NULL, f_customer, (void *)&customer_id[i]);
        printf("0. Customer[id:%d] created\n", i);
    }

    pthread_join(barber_t, NULL);
    for (int i = 0; i < total_customers; i++)
        pthread_join(customers_t[i], NULL);

    
    printf("Customers Left: %d\n", left_customers);

    return 0;



}

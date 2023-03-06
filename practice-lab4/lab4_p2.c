#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define DEPOSIT_THREADS_NUM 7
#define WITHDRAWAL_THREADS_NUM 3
#define NUM_THREADS DEPOSIT_THREADS_NUM + WITHDRAWAL_THREADS_NUM
#define TRANSACTION_AMOUNT 100

// RUNNER FUNCTIONS
void* runner(void *arg);

// BASIC BANK FUNCTIONS
void deposit(int*,int);
void withdraw(int*, int);

typedef struct __bankTransaction_t__ {
    int* bank_balance;
    int amount;
    pthread_mutex_t* mutex;
    sem_t *wait_sem_ptr, *post_sem_ptr;
    void (*transaction_func)(int*, int);
    char* transaction_type;
} bankTransaction_t;

bankTransaction_t* create_bank_transaction(
    int*, int, pthread_mutex_t*, sem_t*, sem_t*, void (*transaction_func)(int*, int), char*
);

int main(int argc, char **argv){
    // read and store command line args
    int 
        bank_balance = 0,
        transaction_amount = atoi(argv[1]);

    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        printf("Error in initializing mutex \n");
    }

    sem_t max_amount_sem, min_amount_sem;
    if (sem_init(&max_amount_sem, 0, 4) !=0){
        printf("Error in initializing empty semaphore \n");
    }
    if (sem_init(&min_amount_sem, 0, 0) !=0){
        printf("Error in initializing empty semaphore \n");
    }
    
    // create threads
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    // execute threads
    for(int i = 0; i < DEPOSIT_THREADS_NUM; i++){
        bankTransaction_t* t = create_bank_transaction(
            &bank_balance,
            TRANSACTION_AMOUNT,
            &mutex,
            &max_amount_sem,
            &min_amount_sem,
            deposit,
            "Deposit"
        );
        
        pthread_create(tid+i, &attr, runner, (void *) t);
    }

    for(int i = 0; i < WITHDRAWAL_THREADS_NUM; i++){
        bankTransaction_t *t = create_bank_transaction(
            &bank_balance,
            TRANSACTION_AMOUNT,
            &mutex, 
            &min_amount_sem,
            &max_amount_sem,
            withdraw, "Withdrawal"
        );
        
        pthread_create(tid+ DEPOSIT_THREADS_NUM + i, &attr, runner, (void *) t);
    }

    // join threads
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    printf("Final amount = %d\n", bank_balance);

    return 0;
}

bankTransaction_t* create_bank_transaction(
    int* bank_balance,
    int amount,
    pthread_mutex_t* mutex,
    sem_t* wait_sem_ptr,
    sem_t* post_sem_ptr,
    void (*transaction_func)(int*, int),
    char* transaction_type
){
    bankTransaction_t* transaction = malloc(sizeof(bankTransaction_t));
    transaction->bank_balance = bank_balance;
    transaction->amount = amount;
    transaction->mutex = mutex;
    transaction->wait_sem_ptr = wait_sem_ptr;
    transaction->post_sem_ptr = post_sem_ptr;
    transaction->transaction_func = transaction_func;
    transaction->transaction_type = transaction_type;
    return transaction;
}

void* runner(void* arg){
    bankTransaction_t *t = (bankTransaction_t *) arg;

    sem_wait(t->wait_sem_ptr);
    pthread_mutex_lock(t->mutex);
    t->transaction_func(t->bank_balance, t->amount);
    printf("%s amount = %d\n", t->transaction_type, *(t->bank_balance));
    sem_post(t->post_sem_ptr);
    pthread_mutex_unlock(t->mutex);

    pthread_exit(0);
}

// deposit function
void deposit(int* bank_balance, int amount){
    printf("Depositing\n");
    *bank_balance += amount;
}

// withdrawal function
void withdraw(int* bank_balance, int amount){
    printf("Withdrawing\n");
    *bank_balance -= amount;
}
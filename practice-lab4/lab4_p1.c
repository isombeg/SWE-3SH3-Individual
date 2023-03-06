#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define DEPOSIT_THREADS_NUM 3
#define WITHDRAWAL_THREADS_NUM 3
#define NUM_THREADS DEPOSIT_THREADS_NUM + WITHDRAWAL_THREADS_NUM

// RUNNER FUNCTIONS
void* runner(void *arg);

// BASIC BANK FUNCTIONS
void deposit(int*, int);
void withdraw(int*, int);

typedef struct __bankTransaction_t__ {
    int* bank_balance;
    int amount;
    pthread_mutex_t* mutex;
    void (*transaction_func)(int*, int);
    char* transaction_type;
} bankTransaction_t;

bankTransaction_t* create_bank_transaction(
    int*, int, pthread_mutex_t*, void (*transaction_func)(int*, int), char*
);

int main(int argc, char **argv){
    // read and store command line args
    int 
        bank_balance = 0,
        deposit_amount = atoi(argv[1]), 
        withdrawal_amount = atoi(argv[2]);

    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        printf("Error in initializing mutex \n");
    }
    
    // create threads
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    // execute threads
    for(int i = 0; i < DEPOSIT_THREADS_NUM; i++){
        bankTransaction_t* t = create_bank_transaction(&bank_balance, deposit_amount, &mutex, deposit, "Deposit");
        
        pthread_create(tid+i, &attr, runner, (void *) t);
    }

    for(int i = 0; i < WITHDRAWAL_THREADS_NUM; i++){
        bankTransaction_t *t = create_bank_transaction(
            &bank_balance, withdrawal_amount, &mutex, withdraw, "Withdrawal"
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
    void (*transaction_func)(int*, int),
    char* transaction_type
){
    bankTransaction_t* transaction = malloc(sizeof(bankTransaction_t));
    transaction->bank_balance = bank_balance;
    transaction->amount = amount;
    transaction->mutex = mutex;
    transaction->transaction_func = transaction_func;
    transaction->transaction_type = transaction_type;
    return transaction;
}

void* runner(void* arg){
    bankTransaction_t *t = (bankTransaction_t *) arg;

    pthread_mutex_lock(t->mutex);
    t->transaction_func(t->bank_balance, t->amount);
    printf("%s amount = %d\n", t->transaction_type, *(t->bank_balance));
    pthread_mutex_unlock(t->mutex);

    pthread_exit(0);
}

// deposit function
void deposit(int* bank_balance, int amount){
    *bank_balance += amount;
}

// withdrawal function
void withdraw(int* bank_balance, int amount){
    *bank_balance -= amount;
}
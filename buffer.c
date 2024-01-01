#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "buffer.h"
#include "Checksum.h"


BUFFER_ITEM buffer[BUFFER_SIZE];
sem_t *empty;
sem_t *full;
pthread_mutex_t mutex;
int count;

void initialize_buffer() {
    empty = sem_open("/empty_sem", O_CREAT, 0644, BUFFER_SIZE); /// Semaphore used to indicate if space if available
    full = sem_open("/full_sem", O_CREAT, 0644, 0); ///Used to indicate if space is full
    count = 0; ///Used to keep track of how many semaphores are available
    if (empty == SEM_FAILED || full == SEM_FAILED) {
        perror("Error creating semaphores");
        exit(1);
    }
    pthread_mutex_init(&mutex, NULL); ///Mutex used to lock and unlock data in the buffer
}

void insert_item(BUFFER_ITEM item) { /// Inserts a BUFFER ITEM into an open buffer spot
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("Error locking the buffer mutex");
        return;
    }

    while (count == BUFFER_SIZE) {
        if (sem_wait(empty) != 0) {
            perror("Error waiting on the empty semaphore");
            pthread_mutex_unlock(&mutex);
            return;
        }
    }

    buffer[count] = item;
    count++;


    if (sem_post(empty) != 0) {
        perror("Error posting to the empty semaphore");
        pthread_mutex_unlock(&mutex);
        return;
    }

    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("Error unlocking the buffer mutex");
        return;
    }

    return;
}

void remove_item(BUFFER_ITEM *item) { /// Removes a BUFFER ITEM from the BUFFER ITEM array
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("Error locking the buffer mutex");
        return;
    }

    while (count == 0) {
        if (sem_wait(full) != 0) {
            perror("Error waiting on the full semaphore");
            pthread_mutex_unlock(&mutex);
            return;
        }
    }

    *item = buffer[count - 1];
    count--;


    if (sem_post(empty) != 0) {
        perror("Error posting to the empty semaphore");
        pthread_mutex_unlock(&mutex);
        return;
    }


    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("Error unlocking the buffer mutex");
        return;
    }

}

void *producer(void *params) { ///Calls the insert_item method
    BUFFER_ITEM item;

    while (1) {
        int sleep_duration = (rand() % 901 + 100);
        usleep(sleep_duration * 1000); // Sleeps for a random period of time before inserting item in buffer
        for (int i = 0; i < 30; i++) {
            item.data[i] = (uint8_t)(rand() % 256);
        }
        item.cksum = checksum(item.data, 30); ///Calculates a checksum for the item being inserted
        insert_item(item);
    }
}

void *consumer(void *params) { ///Calls the remove_item method
    BUFFER_ITEM item;

    while (1) {
        int sleep_duration = (rand() % 901 + 100);
        usleep(sleep_duration * 1000); ///Sleeps for a ranomd period before removing item from buffer
        remove_item(&item);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <delay> <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int delay = atoi(argv[1]); /// Time program will sleep after finishing producer and concumer initialization
    int num_Producer = atoi(argv[2]);
    int num_Consumer = atoi(argv[3]);

    initialize_buffer();

    pthread_t producer_threads[num_Producer];
    pthread_t consumer_threads[num_Consumer];

    for (int i = 0; i < num_Producer; i++) {
        pthread_create(&producer_threads[i], NULL, producer, NULL);
    }
    for (int i = 0; i < num_Consumer; i++) {
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);
    }
    sleep(delay); ///Delay time allows all processes to be completed before terminating the program

    sem_close(empty); ///Closing sempahore before unlinking them
    sem_close(full);

    int result = sem_unlink("/empty_sem"); ///Unlinking sempahores
    if (result != 0) {
        perror("Error unlinking empty semaphore");
    }

    result = sem_unlink("/full_sem");
    if (result != 0) {
        perror("Error unlinking full semaphore");
        exit(1);
    }
    printf("Initialized %d producers\n", num_Producer);
    printf("Initialized %d consumers\n", num_Consumer);
    return 0;
}





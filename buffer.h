#include <stdint.h>

#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 10

typedef struct buffer_item {
    uint8_t data[30];
    uint16_t cksum;
} BUFFER_ITEM;

void initialize_buffer();
void insert_item(BUFFER_ITEM item);
void remove_item(BUFFER_ITEM *item);
void *producer(void *params);
void *consumer(void *params);


#endif


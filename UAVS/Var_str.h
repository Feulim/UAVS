#pragma once
#include "typinclude.h"

#define QUEUE_UNDERFLOW -1


typedef struct node {
    Point data;
    struct node* next;
} node_t;

typedef struct queue {
    node_t* front;
    node_t* rear;
} Queue;


void push(node_t** queue, Point data);
node_t* pop(node_t* queue);
void queue_info(node_t* queue);
void queue_clear(node_t* queue);

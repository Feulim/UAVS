#include "Var_str.h"


void push(node_t** queue, int data) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->data = data;
    new_node->next = NULL;

    if (*queue == NULL) {
        *queue = new_node;
    }
    else {
        node_t* temp = *queue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}


node_t* pop(node_t* queue) {
    if (queue == NULL) {
        exit(QUEUE_UNDERFLOW);
    }
    node_t* temp_next = queue->next;
    free(queue);
    return temp_next;
}


void queue_info(node_t* queue) {
    node_t* temp = queue;
    while (temp != NULL) {
        printf("%d\n", temp->data);
        temp = temp->next;
    }
}


void queue_clear(node_t* queue) {
    while (queue) {
        queue = pop(queue);
    }
}

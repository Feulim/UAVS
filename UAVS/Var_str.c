#include "Var_str.h"


void push(node_t** queue, Point data) {
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

void enqueue(Queue* q, Point p) {
    node_t* newNode = (node_t*)malloc(sizeof(node_t));
    if (!newNode) {
        perror("Failed to allocate memory for queue node");
        exit(EXIT_FAILURE);
    }
    newNode->data = p;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    }
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

Point dequeue(Queue* q) {
    if (q->front == NULL) {
        fprintf(stderr, "Dequeue from empty queue\n");
        exit(EXIT_FAILURE);
    }
    node_t* temp = q->front;
    Point p = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return p;
}

void revers_queue(Queue* q) {
    if (q == NULL || q->front == NULL) {
        return;
    }

    node_t* prev = NULL;
    node_t* current = q->front;
    node_t* next = NULL;
    q->rear = q->front;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    q->front = prev;
}

int is_queue_empty(Queue* q) {
    return q->front == NULL;
}

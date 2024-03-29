#ifndef CQUEUE_H
#define CQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    const char *data;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
    int length;
} Queue;

static void initQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->length = 0;
}

static void enqueue(Queue* queue, const char *data)
{
    Node* newNode = (Node *)malloc(sizeof(Node));
    newNode->data = strdup(data);
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->length++;
}

static void dequeue(Queue  *queue)
{
    if (queue->front == NULL) {
        printf("The queue is empty!\n");
        return;
    }

    Node* temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->length--;
    return;
}

static int isEmpty(Queue *queue)
{
    return (queue->front == NULL);
}

static int queueLength(Queue *queue)
{
    return queue->length;
}

static void clearQueue(Queue *queue)
{
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
}

#endif /* CQUEUE_H */

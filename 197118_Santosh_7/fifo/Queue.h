#ifndef __QUEUE__SAN__
#define __QUEUE__SAN__
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
struct llNode_t {
    int data;
    struct llNode_t *right;
    struct llNode_t *left;
};
typedef struct llNode_t llNode;
typedef llNode *lptr;
void insert_back(lptr *P, int n) {
    lptr T = (lptr)malloc(sizeof(llNode));
    T->data = n;
    T->right = NULL;
    if ((*P) == NULL) {
        (*P) = T;
        (*P)->right=NULL;
        (*P)->left=NULL;
        return;
    }
    (*P)->right = T;
    T->left=(*P);
    (*P) = T;
}
int del_front(lptr *P) {
    int ans = -1;
    if ((*P) != NULL) {
        ans = (*P)->data;
        lptr temp = (*P);
        (*P) = (*P)->right;
        (*P)->left=NULL;
        free(temp);
    }
    return ans;
}
bool isPresentInLL(lptr P,int number){
    while(P){
        if(P->data==number) return true;
        P=P->right;
    }
    return false;
}
void printLL(lptr P){
    while(P){
        printf("%d ",P->data);
        P=P->right;
    }
    printf("\n");
}
struct queue_t {
    lptr f;   //front
    lptr r;   //rear
    int nfe;  //no of elements
};
typedef struct queue_t queue;
void intializeQueue(queue *q) {
    q->f = NULL;
    q->r = NULL;
    q->nfe = 0;
}
void enqueue(queue *q, int ele) {
    insert_back(&(q->r), ele);
    if (q->f == NULL) {
        q->f = q->r;
    }
    q->nfe++;
}
int dequeue(queue *q) {
    int ans = del_front(&(q->f));
    if (q->f == NULL) {
        q->r = NULL;
    }
    if (ans != -1) q->nfe--;
    return ans;
}
bool isPresentInQueue(queue *q,int number){
    return isPresentInLL(q->f,number);
}
void printQueue(queue *q){
    printLL(q->f);

}
#endif

#ifndef __LINKED_LIST__SAN__
#define __LINKED_LIST__SAN__
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
lptr findInLL(lptr P,int page){
    while(P){
        if(P->data==page) return P;
        P=P->right;
    }
    return NULL;
}
int removeLLNode(lptr *P){
    if(!(*P)) return -1;
    if((*P)->left)
    (*P)->left->right=(*P)->right;
    if((*P)->right)
    (*P)->right->left=(*P)->left;
    lptr rightNode=(*P)->right;
    int ans=(*P)->data;
    free((*P));
    (*P)=rightNode;
    return ans;
}
struct frameList_t {
    lptr f;   //front
    lptr r;   //rear
    int nfe;  //no of elements
};
typedef struct frameList_t FrameList;
void intializeFrameList(FrameList *frame) {
    frame->f = NULL;
    frame->r = NULL;
    frame->nfe = 0;
}

lptr findInFrameList(FrameList *frame,int page){
    return findInLL(frame->f,page);
}
void addToFrameList(FrameList *frame, int ele) {
    insert_back(&(frame->r), ele);
    if (frame->f == NULL) {
        frame->f = frame->r;
    }
    frame->nfe++;
}

int deleteFromFrame(FrameList *frame,lptr *P) {
    bool flag=false;
    if((*P)==(frame->f)){
        flag=true;
        frame->f=frame->f->right;
    }
    int ans = removeLLNode(P);
    if (frame->f == NULL) {
        frame->r = NULL;
    }
    if (ans != -1) frame->nfe--;
    return ans;
}
bool isPresentInFrameList(FrameList *frame,int number){
    return isPresentInLL(frame->f,number);
}
void printFrameList(FrameList *frame){
    printLL(frame->f);
}
int deleteLRU(FrameList *frame) {
    int ans = del_front(&(frame->f));
    if (frame->f == NULL) {
        frame->r = NULL;
    }
    if (ans != -1) frame->nfe--;
    return ans;
}
#endif

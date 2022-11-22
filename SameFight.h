#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 52

/*
    This file is for the management of the queue in c, since it's a little more complicated
*/
//int MAX = 52;
int deck [MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

int peek() {
   return deck[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}  

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       
      deck[++rear] = data;
      itemCount++;
   }
}



int removeData() {
   int data = deck[front++];
	
   if(front == MAX) {
      front = 0;
   }
   itemCount--;
   return data;  
}
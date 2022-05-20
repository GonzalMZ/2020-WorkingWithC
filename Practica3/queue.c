


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){
	queue* q=(queue*)malloc(sizeof(queue));
	q->size = size;
	q->elementos = 0;
    	q->insertar = 0;
	q->eliminar = 0;
	q->array = (struct element **)malloc(sizeof(struct element*)*size);
   	return q;
}


// To Enqueue an element
int queue_put(queue* q, struct element* elem) {
	q->array[q->insertar] = (struct element*)malloc(sizeof(struct element));
	memcpy(q->array[q->insertar],elem,sizeof(struct element));
	q->insertar = (q->insertar + 1) % q->size;
	q->elementos = q->elementos + 1;
	return 0;
}


// To Dequeue an element.
struct element* queue_get(queue* q) {
	struct element* element;
	element =(struct element*)  malloc(sizeof(struct element));
	element = q->array[q->eliminar];
	q->eliminar = (q->eliminar + 1) %q->size;
	q->elementos = q->elementos -1;
	return element;
}


//To check queue state
int queue_empty(queue* q){ //si esta vacia devuelve un 1, si no, devuelve un 0
	if(q->elementos==0)
		return 1;
	else
		return 0;	
}


int queue_full(queue* q){ //si esta llena devuelve un 1, si no, devuelve un 0
	if(q->elementos==q->size){	
		return 1;
	}else
		return 0;

}

//To destroy the queue and free the resources
int queue_destroy(queue* q){
	free(q);		
	return 0;
}

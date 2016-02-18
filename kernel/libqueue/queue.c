#include "krnl/libqueue/queue.h"

//#include "spinlock.h"
#include "krnl/kalloc.h"

int lq_new(struct lq_queue* queue){
	queue->head  = 0;
	queue->tail  = 0;
	queue->count = 0;
	initlock(&(queue->lock),"libqueue");
	return 0;
}
void lq_destroy(struct lq_queue* queue){}

void lq_put(struct lq_queue* queue, struct lq_elem* elem){
	acquire(&(queue->lock));
	if(queue->head==0){
		queue->tail = queue->head = elem;
	}else{
		queue->tail->next = elem;
		queue->tail = elem;
	}
	queue->count++;
	release(&(queue->lock));
}
struct lq_elem* lq_get(struct lq_queue* queue){
	struct lq_elem* elem = 0;
	acquire(&(queue->lock));
	if(queue->head){
		elem = queue->head;
		if(queue->head==queue->tail)
			queue->head = queue->tail=0;
		else
			queue->head = elem->next;
		queue->count--;
	}
	release(&(queue->lock));
	return elem;
}



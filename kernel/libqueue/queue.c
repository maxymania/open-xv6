#include "krnl/libqueue/queue.h"

//#include "spinlock.h"
#include "krnl/kalloc.h"

int lq_new(struct lq_queue* queue){
	struct spinlock* lock = (struct spinlock*)kalloc();
	if(lock==0) return -1;
	initlock(lock,"libqueue");
	*queue = (struct lq_queue){lock,0,0};
	return 0;
}
void lq_destroy(struct lq_queue* queue){
	kfree((char*)(queue->lock));
}

void lq_put(struct lq_queue* queue, struct lq_elem* elem){
	acquire(queue->lock);
	if(queue->head==0){
		queue->tail = queue->head = elem;
	}else{
		queue->tail->next = elem;
		queue->tail = elem;
	}
	release((queue->lock));
}
struct lq_elem* lq_get(struct lq_queue* queue){
	struct lq_elem* elem;
	acquire(queue->lock);
	if(queue->head){
		elem = queue->head;
		if(queue->head==queue->tail)
			queue->head = queue->tail=0;
		else
			queue->head = elem->next;
	}
	release(queue->lock);
	return elem;
}



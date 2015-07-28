#ifndef K_LIBQUEUE_QUEUE_H
#define K_LIBQUEUE_QUEUE_H

#include "../spinlock.h"

struct lq_elem {
	void* data;
	struct lq_elem* next;
};

struct lq_queue {
	struct spinlock *lock;
	struct lq_elem* head;
	struct lq_elem* tail;
};

// Returns ==0 on success and !=0 on error.
int             lq_new(struct lq_queue* queue);
// Destroys a queue
void            lq_destroy(struct lq_queue* queue);
// Adds an element
void            lq_put(struct lq_queue* queue, struct lq_elem* elem);
// Removes an element
struct lq_elem* lq_get(struct lq_queue* queue);

#endif


#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#define PGSIZE (4096)
#define NPROC (64)
/*
 * This is where you'll need to implement the user-level functions
 */

void *start[NPROC]; // array used to keep references to all stacks

void lock_init(lock_t *lock) {
	lock->flag = 0;
}

void lock_acquire(lock_t *lock) {
	while(xchg(&lock->flag, 1) != 0);
}

void lock_release(lock_t *lock) {
	xchg(&lock->flag, 0);
}

int thread_join(int pid) {
	lock_t mutex;
	lock_init(&mutex);
	
	lock_acquire(&mutex);
	int t_id = join(pid);
	if (t_id != -1) {
		free(start[t_id]);
	}
	lock_release(&mutex);
	return t_id;
}

int thread_create(void (*start_routine)(void *), void *arg) {
	lock_t mutex;
	lock_init(&mutex);
	
	lock_acquire(&mutex);
	void *stack = malloc(PGSIZE*2);

	if((uint)stack % PGSIZE) {
		stack = stack + (PGSIZE - (uint)stack % PGSIZE);
	}
	int pid = clone(start_routine, arg, stack);
	start[pid%NPROC] = stack; // use pid as an index into the start array
	lock_release(&mutex);

	return pid;
}

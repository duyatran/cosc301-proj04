#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#define PGSIZE (4096)
#define NPROC (64)
#undef NULL
#define NULL ((void*)0)
/*
 * This is where you'll need to implement the user-level functions
 */

sp *start[NPROC]; // array used to keep references to all stacks

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
	int t_id = join(pid);
	if (t_id != -1) {
		int i;
		for (i = 0; i < NPROC; i++) {
			if (start[i]->pid == t_id) {
				free(start[i]->stack);
				free(start[i]);
				break;
			}
		}
	}
	return t_id;
}

int thread_create(void (*start_routine)(void *), void *arg) {
	void *stack = malloc(PGSIZE*2);

	if((uint)stack % PGSIZE) {
		stack = stack + (PGSIZE - (uint)stack % PGSIZE);
	}
	int pid = clone(start_routine, arg, stack);
	if (pid == -1) {
		free(stack);
	}
	else {
	int i;
		for (i = 0; i < NPROC; i++) {
			if (start[i] == NULL) {
				sp *stackptr = malloc(sizeof(sp));
				stackptr->pid = pid;
				stackptr->stack = stack;
				start[i] = stackptr;
				printf(1, "create: pid is %d & stack is %d\n",start[i]->pid,start[i]->stack);
				break;
			}
		}
	}
	return pid;
}

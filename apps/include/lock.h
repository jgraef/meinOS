#ifndef _LOCK_H_
#define _LOCK_H_

typedef int lock_t;

void lock_p(lock_t lock);
void lock_v(lock_t lock);

#endif

#ifndef _SETJMP_H_
#define _SETJMP_H_

#define sigsetjmp(env)      setjmp(env)
#define _setjmp(env)        setjmp(env)
#define siglongjmp(env,val) longjmp(env,val)
#define _longjmp(env,val)   longjmp(env,val)

typedef void** jmp_buf;
typedef jmp_buf sigjmp_buf;

int setjmp(jmp_buf env);
void longjmp(jmp_buf env,int val);

#endif

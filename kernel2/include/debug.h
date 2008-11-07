#ifndef _DEBUG_H_
#define _DEBUG_H_

//#define NODEBUG

#ifndef NOEBUG
  #include <kprint.h>
  #include <sys/cdefs.h>

  #define test(func) { kprintf("%s...",__STRING(func)); kprintf("%s\n",(func)==0?"done":"failed"); }
  #define debug(str) kprintf("%s\n",str!=0?str:"DEBUG")
  #define todo(str) { kprintf("TODO: %s: %d: %s(): %s\n",__FILE__,__LINE__,__func__,str); while (1) cpu_halt(); }
#else
  #define test(func) (func)
  #define debug(str)
  #define todo(str)
#endif

#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  void *addr;
  char type;
  size_t size;
} symbol_t;

static char *get_debug_data() {
  size_t size = 0;
  char *buf = NULL;

  while (!feof(stdin)) {
    buf = realloc(buf,size+BUFSIZ);
    size += fread(buf+size,1,BUFSIZ,stdin);
  }

  return buf;
}

static char *dd_get_proc(char *debug) {
  char *line = strstr(debug,"Process: #");
  if (line!=NULL) {
    char *space = strchr(line+10,' ');
    if (space!=NULL) {
      char *newline = strchr(space,'\n');
      if (newline!=NULL) {
        static char name[256];
        space++;
        memcpy(name,space,newline-space);
        name[newline-space] = 0;
        return name;
      }
    }
  }
  return NULL;
}

static void *dd_get_eip(char *debug) {
  char *pos = strstr(debug,"EIP: ");
  if (pos!=NULL) {
    return (void*)strtoul(pos+5,NULL,16);
  }
  return NULL;
}

static symbol_t *get_symbols(char *name) {
  char *cmd;
  symbol_t *symbols = NULL;

  asprintf(&cmd,"nm -pP --defined-only files/bin/%s",name);
  FILE *nm = popen(cmd,"r");
  free(cmd);
  if (nm!=NULL) {
    size_t i;
    void *addr;
    char type;
    char name[512];
    size_t size;

    for (i=0;fscanf(nm,"%s %c %x %x",name,&type,&addr,&size)>0;i++) {
      symbols = realloc(symbols,(i+1)*sizeof(symbol_t));
      symbols[i].addr = addr;
      symbols[i].name = strdup(name);
      symbols[i].type = type;
      symbols[i].size = size;
      addr = 0;
      type = 0;
      name[0] = 0;
      size = 0;
    }
    symbols = realloc(symbols,(i+1)*sizeof(symbol_t));
    memset(symbols+i,0,sizeof(symbol_t));
    pclose(nm);
    return symbols;
  }
  return NULL;
}

static symbol_t *match_symbol(symbol_t *symbols,void *addr) {
  void *max = 0;
  symbol_t *best = NULL;
  size_t i;

  for (i=0;symbols[i].type!=0;i++) {
    if (addr>symbols[i].addr && symbols[i].addr>max && (symbols[i].type=='t' || symbols[i].type=='T')) {
      max = symbols[i].addr;
      best = symbols+i;
    }
  }
  return best;
}

static void match_stack(char *debug,symbol_t *symbols) {
  size_t i;
  char *stack = strstr(debug,"Stack:\n");
  if (stack!=NULL) {
    char *line = stack+6;

    do {
      void *esp;
      void *addr;

      line++; // skip \n
      sscanf(line,"0x%x: 0x%x",&esp,&addr);

      symbol_t *symbol = match_symbol(symbols,addr);
      if (symbol!=NULL) {
        printf("0x%08x: 0x%08x %s+0x%x (%c,%d)\n",esp,addr,symbol->name,addr-symbol->addr,symbol->type,symbol->size);
      }
      else printf("0x%08x: 0x%08x\n",esp,addr);

    } while ((line = strchr(line,'\n'))!=NULL);
  }
}

static void match_eip(symbol_t *symbols,void *eip) {
  symbol_t *symbol = match_symbol(symbols,eip);
  if (symbol!=NULL) {
    printf("(EIP)       0x%08x %s+0x%x (%c,%d)\n",eip,symbol->name,eip-symbol->addr,symbol->type,symbol->size);
  }
  else printf("(EIP)       0x%08x\n",eip);
}

int main(int argc,char *argv[]) {
  size_t i;
  char *debug = get_debug_data();
  if (debug==NULL) return 1;

  char *proc_name = dd_get_proc(debug);
  if (proc_name==NULL) return 1;
  printf("|======{%s}======[Stacktrace]======|\n",proc_name);

  symbol_t *symbols = get_symbols(proc_name);
  if (symbols==NULL) return 1;

  void *eip = dd_get_eip(debug);
  if (eip!=NULL) match_eip(symbols,eip);

  match_stack(debug,symbols);

  free(debug);
  for (i=0;symbols[i].type!=0;i++) free(symbols[i].name);
  free(symbols);

  return 0;
}

/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <errno.h>
#include <misc.h>

#include <readline/readline.h>
#include <readline/history.h>

#define TERMINAL_DEVICE "/dev/console"

typedef struct {
  const char *cmd;
  int (*func)(char **argv);
} shell_builtin_cmd_t;

typedef struct {
  pid_t pid;
  char *path;
  char **argv;
  int stdin;
  int stdout;
  int stderr;
} shell_proc_t;

struct utsname utsname;
struct passwd *passwd;

static void usage(char *cmd,int err) {
  FILE *out = err?stderr:stdout;
  fprintf(out,"Usage: %s\n");
}

/**
 * Gets command from shell
 *  @return Command line
 */
static char *shell_get_command() {
  char *cwd = getcwd(NULL,0);
  char *prompt;
  char *input;

  asprintf(&prompt,"%s@%s:%s> ",passwd!=NULL?passwd->pw_name:"nobody",utsname.nodename,cwd);
  input = readline(prompt);
  if (input && input[0]!=0) add_history(input);
  else return NULL;

  free(prompt);
  free(cwd);
  return input;
}

/**
 * Converts a string with an octal number to an integer
 *  @param string String holding octal number
 *  @return String as integer
 */
static int octal2num(char *str) {
  char buf[4];
  memcpy(buf,str,3);
  buf[3] = 0;
  int num = strtoul(buf,NULL,8);
  return num;
}

/**
 * Parses an command line argument
 *  @param arg Command line argument
 *  @param len Length of argument
 *  @return Parse arguments
 */
static char *shell_parse_arg(char *arg,size_t len) {
  char *new = malloc(len+1);
  size_t i,j;

  for (i=0,j=0;i<len;i++,j++) {
    if (arg[i]=='\\' && i+1<len) {     // escape code
      i++;
      if (arg[i]=='\\') new[j] = '\\';      // backslash
      else if (arg[i]=='\"') new[j] = '\"'; // quotation mark
      else if (arg[i]=='a') new[j] = '\a';  // alert
      else if (arg[i]=='b') new[j] = '\b';  // backspace
      else if (arg[i]=='f') new[j] = '\f';  // form feed
      else if (arg[i]=='n') new[j] = '\n';  // new line
      else if (arg[i]=='r') new[j] = '\r';  // carriage return
      else if (arg[i]=='t') new[j] = '\t';  // horizontal tab
      else if (arg[i]=='v') new[j] = '\v';  // vertical tab
      else if (arg[i]=='0' && arg[i+1]!=0 && arg[i+2]!=0 && arg[i+3]!=0) {
                                            // character in octal
        new[j] = octal2num(arg+i+1);
      }
    }
    else new[j] = arg[i];
  }

  new[j] = 0;

  return new;
}

/**
 * Parses a command line string
 *  @param cmd Command line string
 *  @return Argument vector
 */
static char **shell_parse_cmd(char *cmd) {
  int quote = 0;
  char *cur = cmd;
  char *next;
  char **argv = NULL;
  int argc = 0;

  // skip beginning spaces
  while (*cur==' ') cur++;

  do {
    // find next space (or quotation mark)
    next = strchr(cur,quote?'\"':' ');
    if (next==NULL) next = cmd+strlen(cmd);
    // if next character is a quotation mark, activate qoute mode
    //if (*next=='\"') mode = 1;

    // skip spaces
    //while (*next==' ') next++;

    // add current to argv
    argv = realloc(argv,(argc+1)*sizeof(char*));
    argv[argc++] = shell_parse_arg(cur,next-cur);
    cur = next+1;
  } while (*next!=0);

  argv = realloc(argv,(argc+1)*sizeof(char*));
  argv[argc] = NULL;

  return argv;
}

static int shell_builtin_exit(char **argv) {
  return 1;
}

static int shell_builtin_cd(char **argv) {
  if (argv[1]!=NULL) {
    if (chdir(argv[1])==-1) fprintf(stderr,"sh: cd: %s: %s\n",argv[1],strerror(errno));
  }
  return 0;
}

static int shell_builtin_help(char **argv) {
  printf("Built-in commands:\n"
         " exit              Exit shell\n"
         " cd DIR            Change working directory to DIR\n"
         " help              Show this help dialog\n"
         " version           Show version\n");
  return 0;
}

static int shell_builtin_version(char **argv) {
  printf("%s %s\n",utsname.sysname,utsname.version);
  return 0;
}

static int shell_run_builtin(char **argv) {
  shell_builtin_cmd_t shell_builtin_cmds[] = {
    { .cmd = "exit",    .func = shell_builtin_exit },
    { .cmd = "cd",      .func = shell_builtin_cd },
    { .cmd = "help",    .func = shell_builtin_help },
    { .cmd = "version", .func = shell_builtin_version }
  };
  size_t i;

  for (i=0;i<sizeof(shell_builtin_cmds)/sizeof(shell_builtin_cmd_t);i++) {
    if (strcmp(argv[0],shell_builtin_cmds[i].cmd)==0) return shell_builtin_cmds[i].func(argv);
  }

  return -1;
}

static char *shell_find_path(char *cmd) {
  char *path;
  /// @todo read from PATH enviroment variable
  asprintf(&path,"/boot/bin/%s",cmd);
  return path;
}

static int shell_run_binary(char **argv,int background) {
  shell_proc_t *proc = malloc(sizeof(shell_proc_t));
  proc->path = shell_find_path(argv[0]);
  proc->argv = argv;
  proc->pid = execute(proc->path,argv,&(proc->stdin),&(proc->stdout),&(proc->stderr));

  if (proc->pid==-1) {
    free(proc->path);
    free(proc);
    return -1;
  }

  if (background) {

  }
  else {
    int status = 0;
    waitpid(proc->pid,&status,0);

    close(proc->stdin);
    close(proc->stdout);
    close(proc->stderr);
    free(proc->path);
    free(proc);

    if (status!=0) fprintf(stderr,"sh: %s: returned with %d\n",argv[0],status);
  }

  return 0;
}

/**
 * Runs shell interactive
 */
static void shell_interactive() {
  int status = 0;

  while (status!=1) {
    size_t i;
    char **argv;
    char *cmd = shell_get_command();

    if (cmd==NULL) break;
    argv = shell_parse_cmd(cmd);

    if ((status = shell_run_builtin(argv))==-1) {
      if (shell_run_binary(argv,0)==-1) fprintf(stderr,"sh: %s: command not found\n",argv[0]);
    }

    free(cmd);
    for (i=0;argv[i];i++) free(argv[i]);
    free(argv);
  }
}

int main(int argc,char *argv[]) {
  int c;

  FILE *terminal = fopen(TERMINAL_DEVICE,"r+");
  if (terminal==NULL) return 1;
  FILE *stdin_bak = stdin;
  FILE *stdout_bak = stdout;
  FILE *stderr_bak = stderr;
  stdin = terminal;
  stdout = terminal;
  stderr = terminal;

  while ((c = getopt(argc,argv,":hv"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("sh v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  uname(&utsname);
  passwd = getpwuid(getuid());
  shell_interactive();

  stdin = stdin_bak;
  stdout = stdout_bak;
  stderr = stderr_bak;
  fclose(terminal);

  return 0;
}

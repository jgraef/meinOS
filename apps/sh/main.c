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
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <llist.h>
#include <misc.h>

#include <readline/readline.h>
#include <readline/history.h>

typedef struct {
  const char *cmd;
  int (*func)(char **argv);
} shell_builtin_cmd_t;

typedef struct {
  pid_t pid;
  char *path;
  char **argv;
  char *stdin;
  char *stdout;
  char *stderr;
} shell_job_t;

static struct utsname utsname;
static struct passwd *passwd;
static llist_t joblist;
static char *terminal_device;

static void usage(char *cmd,int err) {
  FILE *out = err?stderr:stdout;
  fprintf(out,"Usage: %s\n");
  exit(err);
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
  if (input!=NULL) {
    if (input[0]!=0) add_history(input);
  }
  else return NULL;

  free(prompt);
  free(cwd);
  return input;
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
      else if (arg[i]=='0') {              // character in octal
        char *endp;
        int num = strtoul(arg+i,&endp,8);
        if (errno==0) {
          new[j] = num;
          i += (endp-(arg+i))-1;
        }
        else {
          new[j] = arg[i];
          perror("strtoul");
        }
      }
      else new[j] = '\\';
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
static char **shell_parse_cmd(char *cmd,int *background) {
  int quote = 0;
  char *cur = cmd;
  char *next;
  char **argv = NULL;
  int argc = 0;
  *background = 0;

  do {
    // skip spaces
    while (*cur==' ') cur++;
    if (*cur==0) break;

    // find next space (or quotation mark)
    next = strchr(cur,quote?'\"':' ');
    if (next==NULL) next = cmd+strlen(cmd);

    // if next character is a quotation mark, activate qoutation mode
    //if (*next=='\"') qoute = 1;

    // add current to argv
    argv = realloc(argv,(argc+1)*sizeof(char*));
    argv[argc++] = shell_parse_arg(cur,next-cur);
    cur = next+1;
  } while (*next!=0);

  if (argc>0) {
    if (strcmp(argv[argc-1],"&")==0) {
      *background = 1;
      argc--;
      argv[argc] = NULL;
    }
    else {
      argv = realloc(argv,(argc+1)*sizeof(char*));
      argv[argc] = NULL;
    }
    return argv;
  }
  else return NULL;
}

static int shell_builtin_exit(char **argv) {
  return 1;
}

static int shell_builtin_help(char **argv) {
  printf("Built-in commands:\n"
         " exit              Exit shell\n"
         " cd DIR            Change working directory to DIR\n"
         " ls [DIR]          List content of DIR\n"
         " help              Show this help dialog\n"
         " version           Show version\n");
  return 0;
}

static int shell_builtin_version(char **argv) {
  printf("%s %s\n",utsname.sysname,utsname.version);
  return 0;
}

static int shell_builtin_cd(char **argv) {
  if (argv[1]!=NULL) {
    if (chdir(argv[1])==-1) fprintf(stderr,"sh: cd: %s: %s\n",argv[1],strerror(errno));
  }
  return 0;
}

static char ls_filetype(mode_t mode) {
  if (S_ISBLK(mode)) return 'b';
  else if (S_ISCHR(mode)) return 'c';
  else if (S_ISDIR(mode)) return 'd';
  else if (S_ISFIFO(mode)) return 'f';
  else if (S_ISLNK(mode)) return 'l';
  else if (S_ISSOCK(mode)) return 's';
  else return '-';
}

static char *ls_perm(mode_t mode) {
  static char buf[10];
  snprintf(buf,10,"%c%c%c%c%c%c%c%c%c",mode&S_IRUSR?'r':'-',mode&S_IWUSR?'w':'-',mode&S_IXUSR?'x':'-',mode&S_IRGRP?'r':'-',mode&S_IWGRP?'w':'-',mode&S_IXGRP?'x':'-',mode&S_IROTH?'r':'-',mode&S_IWOTH?'w':'-',mode&S_IXOTH?'x':'-');
  return buf;
}

static int shell_builtin_ls(char **argv) {
  char *path = argv[1]==NULL?".":argv[1];

  DIR *dir = opendir(path);
  if (dir!=NULL) {
    struct dirent *dirent;
    do {
      dirent = readdir(dir);
      if (dirent!=NULL) {
        if (strcmp(dirent->d_name,".")!=0 && strcmp(dirent->d_name,"..")!=0) {
          struct stat stbuf;
          struct passwd *owner;
          char *file;
          asprintf(&file,"%s/%s",path,dirent->d_name);
          if (stat(file,&stbuf)==-1) printf("Error (%d): %s\n",errno,strerror(errno));
          owner = getpwuid(stbuf.st_uid);
          printf("%c%s %s % 5d %s\n",ls_filetype(stbuf.st_mode),ls_perm(stbuf.st_mode),owner!=NULL?owner->pw_name:"root",stbuf.st_size,dirent->d_name);
        }
      }
    } while (dirent!=NULL);
    return 0;
  }
  else return 1;
}

static int shell_builtin_pwd(char **argv) {
  char *cwd = getcwd(NULL,0);
  puts(cwd);
  free(cwd);
  return 0;
}

static int shell_builtin_cls(char **argv) {
  fputs("\x1B[2J",stdout);
  return 0;
}

static int shell_run_builtin(char **argv) {
  shell_builtin_cmd_t shell_builtin_cmds[] = {
    { .cmd = "exit",     .func = shell_builtin_exit },
    { .cmd = "logout",   .func = shell_builtin_exit },
    { .cmd = "help",     .func = shell_builtin_help },
    { .cmd = "version",  .func = shell_builtin_version },
    { .cmd = "cd",       .func = shell_builtin_cd },
    { .cmd = "ls",       .func = shell_builtin_ls },
    { .cmd = "pwd",      .func = shell_builtin_pwd },
    { .cmd = "cls",      .func = shell_builtin_cls }
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
  shell_job_t *job = malloc(sizeof(shell_job_t));
  job->path = shell_find_path(argv[0]);
  job->argv = argv;
  job->stdin = terminal_device;
  job->stdout = terminal_device;
  job->stderr = terminal_device;
  job->pid = execute(job->path,argv,job->stdin,job->stdout,job->stderr);

  if (job->pid==-1) {
    free(job->path);
    free(job);
    return -1;
  }

  if (background) {
    llist_push(joblist,job);
    printf("+[%d]\n",job->pid);
  }
  else {
    int status;
    waitpid(job->pid,&status,0);

    free(job->path);
    free(job);

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
    char *cmd;
    int background = 0;

    if ((cmd = shell_get_command())==NULL) break;

    if ((argv = shell_parse_cmd(cmd,&background))!=NULL) {
      if ((status = shell_run_builtin(argv))==-1) {
        if (shell_run_binary(argv,background)==-1) fprintf(stderr,"sh: %s: command not found\n",argv[0]);
      }

      for (i=0;argv[i];i++) free(argv[i]);
      free(argv);
    }

    free(cmd);
  }
}

int main(int argc,char *argv[]) {
  int c;

  terminal_device = "/dev/console";

  while ((c = getopt(argc,argv,":hvw:t:"))!=-1) {
    switch(c) {
      case 'w':
        chdir(optarg);
        break;
      case 't':
        terminal_device = optarg;
        break;
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

  FILE *terminal = fopen(terminal_device,"r+");
  if (terminal==NULL) return 1;
  FILE *stdin_bak = stdin;
  FILE *stdout_bak = stdout;
  FILE *stderr_bak = stderr;
  stdin = terminal;
  stdout = terminal;
  stderr = terminal;

  uname(&utsname);
  passwd = getpwuid(getuid());
  shell_interactive();

  stdin = stdin_bak;
  stdout = stdout_bak;
  stderr = stderr_bak;
  fclose(terminal);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/history.h>

#include "Command.h"
#include "error.h"

typedef struct {
  char *file;
  char **argv;
} *CommandRep;

#define BIARGS CommandRep r, int *eof, Jobs jobs
#define BINAME(name) bi_##name
#define BIDEFN(name) static void BINAME(name) (BIARGS)
#define BIENTRY(name) {#name,BINAME(name)}

static char *owd=0; 
static char *cwd=0;

static void builtin_args(CommandRep r, int n) {
  char **argv=r->argv;
  for (n++; *argv++; n--);
  if (n)
    ERROR("wrong number of arguments to builtin command"); // warn
}

/**
 * Exits program, should wait for child programs to finish before executing
 */
BIDEFN(exit) {
  builtin_args(r,0);
  *eof=1;
}

/**
 * Prints the current working directory
 */
BIDEFN(pwd) {
  builtin_args(r,0);
  if (!cwd)
    cwd=getcwd(0,0);
  printf("%s\n",cwd);
}

/**
 * Prints out command history
 */
BIDEFN(history) {
  builtin_args(r,0);
  HIST_ENTRY **list = history_list();

  if (list == NULL) {
    printf("No history available.\n");
    return;
  }

  for (int i = 0; list[i] != NULL; i++) {
    printf("%d: %s\n", history_base + i, list[i]->line);
  }
}

/**
 * Changes the current working directory to a user-specified directory
 * 
 * Checks first if the directory path exits in current directory, then tries from ~ (home)
 */
BIDEFN(cd) {
  builtin_args(r,1);

  char *target = r->argv[1];
  char path[1024];

  char *home = getenv("HOME");

  // Handle "-" (go back to previous directory before last directory change)
  if (strcmp(target,"-")==0) {
    if (!owd) {
      ERROR("cd: Old working directory (owd) not set");
      return;
    }
    snprintf(path, sizeof(path), "%s", owd);
  }

  // "~" specifier for home
  else if (target[0] == '~') {
    if (!home) {
        ERROR("cd: $HOME not set");
        return;
    }
    /* just ~ */      if (target[1] == '\0') snprintf(path, sizeof(path), "%s", home);
    /* ~/...  */ else if (target[1] == '/')  snprintf(path, sizeof(path), "%s%s", home, target + 1);
    /* other  */                        else snprintf(path, sizeof(path), "%s", target);
    
  }

  // Relative or absolute
  else {
    snprintf(path, sizeof(path), "%s", target);
  }

  /**
   * Now change the actual process working directory
   */
  if (chdir(path) == 0) {
    if (owd) free(owd);
    owd=cwd;
    cwd= getcwd(NULL, 0);
  } else {
    ERROR("chdir() failed"); // warn
  }
}


/**
 * Specifies the built in commands the shell will be using
 */
static int builtin(BIARGS) {
  typedef struct {
    char *s;
    void (*f)(BIARGS);
  } Builtin;
  static const Builtin builtins[]={
    BIENTRY(exit),
    BIENTRY(pwd),
    BIENTRY(history),
    BIENTRY(cd),
    {0,0}
  };
  int i;
  for (i=0; builtins[i].s; i++)
    if (!strcmp(r->file,builtins[i].s)) {
      builtins[i].f(r,eof,jobs);
      return 1;
    }
  return 0;
}

static char **getargs(T_words words) {
  int n=0;
  T_words p=words;
  while (p) {
    p=p->words;
    n++;
  }
  char **argv=(char **)malloc(sizeof(char *)*(n+1));
  if (!argv)
    ERROR("malloc() failed");
  p=words;
  int i=0;
  while (p) {
    argv[i++]=strdup(p->word->s);
    p=p->words;
  }
  argv[i]=0;
  return argv;
}

extern Command newCommand(T_words words) {
  CommandRep r=(CommandRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->argv=getargs(words);
  r->file=r->argv[0];
  return r;
}

static void child(CommandRep r, int fg) {
  int eof=0;
  Jobs jobs=newJobs();
  if (builtin(r,&eof,jobs))
    return;
  execvp(r->argv[0],r->argv);
  ERROR("execvp() failed");
  exit(0);
}

extern void execCommand(Command command, Pipeline pipeline, Jobs jobs,
			int *jobbed, int *eof, int fg) {
  CommandRep r=command;
  if (fg && builtin(r,eof,jobs))
    return;
  if (!*jobbed) {
    *jobbed=1;
    addJobs(jobs,pipeline);
  }
  int pid=fork();
  if (pid==-1)
    ERROR("fork() failed");
  if (pid==0) {
    child(r,fg);
  } else {
    if (fg) {
      int status;
      waitpid(pid, &status, 0);
    }
  }
    
}

extern void freeCommand(Command command) {
  CommandRep r=command;
  char **argv=r->argv;
  while (*argv)
    free(*argv++);
  free(r->argv);
  free(r);
}

extern void freestateCommand() {
  if (cwd) free(cwd);
  if (owd) free(owd);
}

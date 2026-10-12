#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Pipeline.h"
#include "deq.h"
#include "error.h"

typedef struct {
  Deq processes;
  int fg;			// not "&"
} *PipelineRep;

extern Pipeline newPipeline(int fg) {
  PipelineRep r=(PipelineRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->processes=deq_new();
  r->fg=fg;
  return r;
}

extern void addPipeline(Pipeline pipeline, Command command) {
  PipelineRep r=(PipelineRep)pipeline;
  deq_tail_put(r->processes,command);
}

extern int sizePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  return deq_len(r->processes);
}

static void execute(Pipeline pipeline, Jobs jobs, int *jobbed, int *eof) {
  PipelineRep r=(PipelineRep)pipeline;

  int cmdamt = sizePipeline(r);
  if (cmdamt == 0) return;

  pid_t pids[cmdamt];
  int pidsamt = 0;

  int infd = STDIN_FILENO;
  int pipefds[2];

  for (int i=0; i<cmdamt && !*eof; i++) {
    Command cmd = deq_head_ith(r->processes, i);

    if (i < cmdamt - 1) {
      if (pipe(pipefds) < 0) {
        ERROR("pipe() failed");
        return;
      }
    }

    int outfd = (i < cmdamt - 1) ? pipefds[1] : STDOUT_FILENO;

    pid_t pid = execCommand(cmd, pipeline, jobs, jobbed, eof, r->fg, infd, outfd);

    if (pid > 0) pids[pidsamt++] = pid;

    if (outfd != STDOUT_FILENO) close(pipefds[1]);
    if (infd != STDIN_FILENO) close(infd);

    if (i < cmdamt - 1) infd = pipefds[0];
  }

  if (r->fg) {
    for (int i = 0; i < pidsamt; i++) {
      int status;
      waitpid(pids[i], &status, 0);
    }
  }
  
}

extern void execPipeline(Pipeline pipeline, Jobs jobs, int *eof) {
  int jobbed=0;
  execute(pipeline,jobs,&jobbed,eof);
  if (!jobbed)
    freePipeline(pipeline);	// for fg builtins, and such
}

extern void freePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  deq_del(r->processes,freeCommand);
  free(r);
}

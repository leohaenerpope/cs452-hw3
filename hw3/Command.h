#ifndef COMMAND_H
#define COMMAND_H

typedef void *Command;

#include <sys/types.h>
#include "Tree.h"
#include "Jobs.h"
#include "Sequence.h"


extern Command newCommand(T_words words);

extern pid_t execCommand(Command command, Pipeline pipeline, Jobs jobs,
			int *jobbed, int *eof, int fg, int infd, int outfd);

extern void freeCommand(Command command);
extern void freestateCommand();

#endif

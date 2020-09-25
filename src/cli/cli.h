#ifndef __CLI__
#define __CLI__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define CMDLEN 12
#define CMDDIV 3
#define PS1  "> "

void notcmd(void *data);
void cmd_hello(void *data);
void cmd_getCmd(void *data);
void cmd_listcmds(void *data);
void cli_init();
void cli_enableRawMode();
void cli_keyHandle();
void *cli_start(void *data);

#endif

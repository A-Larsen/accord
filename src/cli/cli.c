#include "cli.h"

#define CTRL_KEY(k) ((k) & 0x1f)

typedef void (*cmd_t)(void *);

// eval might be a good thing to use 
// to look a memory in program

char cmd[255] = {};
int cmdind = 0;
int PS1LEN = 0;

extern const void *COMMANDS[CMDLEN];

void
output_cmd(str)
  char *str;
{
	write(STDOUT_FILENO, "\r\n", 2);
	write(STDOUT_FILENO, str, strlen(str));
}

void 
cmd_hello(data)
  void *data;
{
	output_cmd("you entered hello cmd !");
}

void
notcmd(data)
  void *data;
{
	output_cmd("not a command");
}

void
cmd_getCmd(data)
  void *data;
{
	output_cmd(cmd);
}


void
cmd_listcmds(data)
	void *data;
{
	char *cmd1 = NULL;
	char *cmd2 = NULL;
	for(int i = 0; i < CMDLEN; i++){

		if(i%CMDDIV == 0){
			cmd1 = (char *)COMMANDS[i];
		}

		if(i%CMDDIV == 2){
			cmd2 = (char *)COMMANDS[i];
		}

		if(cmd1 && cmd2){
			char *str = NULL;
			write(STDOUT_FILENO, "\r\n", 2);

			int len = asprintf(&str, "%s\t\t%s\n", cmd1, cmd2);

			write(STDOUT_FILENO, str, len);

			cmd1 = NULL;
			cmd2 = NULL;
		}
	}
}


void *
searchcmds(query)
  char *query;
{
	for(int i = 0; i < CMDLEN; i++){
		if(i%CMDDIV == 0){
			if(!strcmp(query, COMMANDS[i])){
				return (void *)COMMANDS[i+1];
			}
		}
	}
	return NULL;
}

struct termios orig_termios;


void die(const char *s){
	perror(s);
	exit(1);
}

void disableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void cli_enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");

	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw))
		die("tcsetattr");
}


void
getCommand(cmd)
  char *cmd;
{
	cmd_t fun = searchcmds(cmd);
	searchcmds(cmd);

	if(fun){
		fun(NULL);
	}else{
		((cmd_t)searchcmds("none"))(NULL);

	}

	write(STDOUT_FILENO, "\r\n", 2);
	write(STDOUT_FILENO, PS1, 2);
}

int
readKey()
{
	char c = 0;
	if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
		die("read");

	return c;
}

void 
cli_clear(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	write(STDOUT_FILENO, PS1, 2);
}

void
cli_keyHandle()
{
	char c = readKey();

	switch(c){

		case 13:{
			cmd[cmdind++] = 0;
			getCommand(cmd);
			cmdind = 0;

			break;
		}

		case CTRL_KEY('c'): 
		case CTRL_KEY('d'): {
			write(STDOUT_FILENO, "exiting\r\n", 9);
			exit(0);
			break;
		}
		case CTRL_KEY('l'): {
			cli_clear();
			break;
		}

		case 127: {
			if((cmdind+PS1LEN) > PS1LEN){
				cmdind--;
				write(STDOUT_FILENO, "\x1b[D", 3);
				write(STDOUT_FILENO, " ", 1);
				write(STDOUT_FILENO, "\x1b[D", 3);

			}
			break;
	  	}

		default: if(c != 0) {
			char str[1] = {c};

			cmd[cmdind++] = c;
			write(STDOUT_FILENO, str, 1);
			 break;
		 }
	}
}


void
cli_init()
{
	PS1LEN = strlen(PS1);
	cli_clear();
}

void *
cli_start(data)
  void *data;
{
	cli_enableRawMode();
	cli_init();

	while(1){
		cli_keyHandle();
	}

	return NULL;
}


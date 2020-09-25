#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include "cli/cli.h"
#include <pthread.h>

#include "server/server.h"

// if you include this in another c file define this at the top of
// that file
//
const void *
COMMANDS[CMDLEN] = {
	// name        function        description
	"none",        notcmd,         "not a commad.. or is it ?",
	"hello",       cmd_hello,      "hello description",
	"getcmd",      cmd_getCmd,     "cmd_getCmd",
	"help",        cmd_listcmds,    "list all commads",
};

onion *o = NULL;
char OPTIONS = 0;
char *OPTION_VALUE = NULL;

static void shutdown_server(int _) {
	ONION_INFO("ending");
	if (o)
		onion_listen_stop(o);
}

int 
main(int argc, char **argv)
{
	char *port = NULL;

	int ch;

	while((ch = getopt(argc, argv, "cqa:p:")) != -1){

		switch(ch){

			case 'a':
				OPTIONS |= OPTIONAUTOLOGIN;
				char * val = strdup(optarg);
				OPTION_VALUE = val;
				OPTION_VALUE[strlen(val)] = 0;
				break;
		
			case 'p':
				port = strdup(optarg);
				break;

			case 'q':
				putenv("ONION_LOG=noinfo");
				break;

			case 'c':
				OPTIONS |= OPTIONCLI;
				putenv("ONION_LOG=noinfo");
				break;

			case '?':
				fprintf(stderr, "usage: %s [-a admin] [-p port]\n", argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	signal(SIGINT, shutdown_server);
	signal(SIGTERM, shutdown_server);


	o = onion_new(O_THREADED);

	if(port)
		onion_set_port(o, port);

	free(port);

	onion_url *urls = onion_root_url(o);
	server_init(urls);

	onion_url_add(urls, "", server_connection_home);
	onion_url_add(urls, "chat", server_connection_chat);

	onion_url_add(urls, "login/data", server_connection_login);
	onion_url_add(urls, "signup/data", server_connection_signup);
	onion_url_add(urls, "sitemap.xml", server_connection_sitemap);


	onion_url_add_static(urls, "login",
			server_get_view("login.html"), HTTP_OK);


	onion_url_add_static(urls, "signup", 
			server_get_view("signup.html"), HTTP_OK);

	if(OPTIONS & OPTIONCLI){
		pthread_t newthread;
		pthread_create(&newthread, NULL, cli_start, NULL);
	}

	onion_listen(o);

	onion_free(o);
	server_free();
	return 0;


}

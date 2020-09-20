#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <signal.h>
#include <getopt.h>

#include "server/server.h"

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
	int ch;

	bool badOption = false;
	while((ch = getopt(argc, argv, "a:")) != -1){

		switch(ch){
			case 'a':
				OPTIONS |= AUTOLOGIN;
				/* printf("OPTION: %s", argv[optind -1]); */
				char * val = strdup(argv[optind - 1]);
				OPTION_VALUE = val;
				OPTION_VALUE[strlen(val)] = 0;
				break;

		}
	}

	if(badOption)
		return -1;

	signal(SIGINT, shutdown_server);
	signal(SIGTERM, shutdown_server);

	o = onion_new(O_THREADED);

	/* onion_set_port(o, "80"); */
	onion_url *urls = onion_root_url(o);
	server_init(urls);

	onion_url_add(urls, "", server_connection_home);
	onion_url_add(urls, "chat", server_connection_chat);

	onion_url_add(urls, "login/data", server_connection_login);
	onion_url_add(urls, "signup/data", server_connection_signup);
	onion_url_add(urls, "sitemap.xml", server_connection_sitemap);

	
	char * loginview = strdup(server_get_view("login.html"));

	if(!loginview){
		fprintf(stderr, "could not read login.html");
	}

	onion_url_add_static(urls, "login", loginview, HTTP_OK);

	char * signupview = strdup(server_get_view("signup.html"));

	if(!signupview){
		fprintf(stderr, "cound not read signup.html");
	}

	onion_url_add_static(urls, "signup", signupview, HTTP_OK);

	free(loginview);
	free(signupview);

	onion_listen(o);

	onion_free(o);
	server_free();
	return 0;


}

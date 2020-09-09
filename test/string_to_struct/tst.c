#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct thing {

	char *name;
	int what;
};

void fun(struct thing *a){
	/* a->name = malloc(sizeof(char) * 4); */
	/* char *name = malloc(sizeof(char) *4) */
	char name[100];
	char *np = name;
	*np++ = 'a';
	*np++ = 'b';
	*np++ = 'c';
	*np = 0;

	a->name = strdup(name);
	/* a->name = "no"; */
	/* a */
	/* a->what = b; */
}
int main(){
	struct thing one;
	/* one.name = "yep"; */
	fun(&one);

    printf("%s\n", one.name);
	char *b;
	b = strdup("what");
	printf("%s\n", b);
}

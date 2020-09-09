#include <stdlib.h>
#include <stdio.h>

struct things {
	int num;
};

/* #define CASE1 */ 
/* #define CASE2 */
#define CASE3

int main(){

#ifdef CASE1
	struct things *a;
	a = (things *)malloc(sizeof(struct things));
	a->num = 12;
	struct things *b;
	b = a;
	/* printf("a address: %p\nb address: %p\n",&a, &b); */
	printf("a address: %p\nb address: %p\n",a, b);
	// this printf proves that b is a copy of a and NOT pointing to the same
	// address..
	// or maybe not
	free(a);
	b->num = 64;

	printf("%d\n", b->num);
	// despite the fact that I set a to NULL I can still access the information
	// inside of a like this. why ?
	// b must be a copy of a and not shard the same address
#endif
	

#ifdef CASE2

	char *str1 = "hello";

	char *str2;
	str2 = str1;

	free(str1);
	printf("str1: %p\nstr2: %p\n", str1, str2);

	// in this case I cannot access the memory.. but in the case above I can...
	//

	// my guess is that is simply because I am setting str1 to a constant
	// maybe it doesn't get put on the heap if I set a pointer to a constant
	// lets try this
#endif

#ifdef CASE3
	char *str1 = malloc(sizeof(char) * 5);
	str1 = "hello";
	char *str2;
	str2 = str1;
	free(str1);
	printf("%s\n", str2);
	// nope. I still can't acces the memory. and that makes sense.. but it
	// doesn't make sense in case1
#endif
}



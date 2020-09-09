#include <stdlib.h>
#include <stdio.h>

int main(){
	void *data = NULL;

	int initsize = sizeof(int) + sizeof(long int);
	data = malloc(initsize);

	((int *)(data))[0] = 12;;
	((long int *)(data))[1] = 13;;
	((char ***)(data))[2] = malloc(sizeof(char) * 4);
	((char ***)(data))[2][0] = "hiya";

	((char ***)(data))[2] = realloc( ((char ***)(data))[2],  sizeof(char) * 4 + sizeof(char) * 5);

	((char ***)(data))[2][1] = "hello";



    /* printf("hello people\n"); */
    printf("%d\n", ((int *)(data))[0]);
    printf("%ld\n", ((long int *)(data))[1]);
    printf("%s\n", ((char ***)(data))[2][0]);
    printf("%s\n", ((char ***)(data))[2][1]);
}

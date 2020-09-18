#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

char * arrayToJSONArray(char **a, int len)
{
	char js[50] = "[";
	char *jsp = js + 1;

	int check = len - 1;
	for(int i = 0; i < len; i++){

		*jsp++ = '\"';

		int len = strlen(a[i]);

		strncpy(jsp, a[i], len);
		jsp += len;
		*jsp++ = '\"';

		// order of operations must prevent this
		/* if( i < (len - 1) ){ */
		if( i < check ){
			*jsp++ = ',';

		}

	}

	*jsp++ = ']';
	*jsp = 0;

	return strdup(js);
}

int main(){
	char *list[] = {"one", "two", "three"};

	/* for(; *list != 0; (*list)++){ */
	/* 	printf("%s\n", *list); */
	/* } */
	/* printf("%s\n", list[0]); */
	char *a = arrayToJSONArray(list, 3);
	printf("%s\n", a);

}

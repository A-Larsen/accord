#include <stdlib.h>
#include <stdio.h>

void pointermove(char *a){
	a+=4;
	printf("in function: %10s\n", a);
}

int pointermove2(char *a){
	a+=4;
	printf("in function: %10s\n", a);
	return 4;
}

int main(){
	char *a = "what is up";
	pointermove(a);
    printf("in main: %15s\n", a);
	a += pointermove2(a);
    printf("in main: %10s\n", a);
}

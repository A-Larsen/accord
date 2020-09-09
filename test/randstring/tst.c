#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

char getRandchar(){
	return (char)floor((float)rand() / RAND_MAX * 26) + 'a';
}

void getRandStr(char a[20]){

	int i = 0;

	for(i = 0; i < 20; i++){
		a[i] = getRandchar();
	}

	a[i] = 0;

}

int main(){
	srand(time(0));

	char a[20];
	getRandStr(a);

	printf("%s\n", a);


}

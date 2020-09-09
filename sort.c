#include "sort.h"

void 
sortn(data, len)
  void **data;
  int len;
{
	for(int i = 0; i < len; i++){
		for(int y = 0; y < len; y++){
			if(!data[y]){
				data[y] = data[i];
				data[i] = NULL;
			}
		}
	}

	for(int i = 0; i < len; i++){
		if(!data[i]){
			free(data[i]);
		}
	}
}

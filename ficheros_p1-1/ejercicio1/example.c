#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if (argc<3){
	fprintf(stderr,"Usage: %s <first-name> <last-name>\n",argv[0]);	
	return 1;
  }

  printf("Hi %s %s!\n",argv[1],argv[2]);
  return 0;
}



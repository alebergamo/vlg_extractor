#include <stdio.h>
#include "utils.h"

int main(int argc, char* argv[]) {
	printf("This is the mainfunction\n");
	
	char* c = "CIAO/FILE.TXT";
	char* c2 = NULL;
	
	c2 = dirname(c);
	printf("%s\n",c2);
 	
	return 0;
}

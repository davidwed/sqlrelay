#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void checkSuccess(char *value, char *success) {
	//printf("\"%s\"=\"%s\"\n",value,success);

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("failure ");
		exit(0);
	}
}

void checkSuccess(int value, int success) {
	//printf("\"%d\"=\"%d\"\n",value,success);

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		exit(0);
	}
}

int	main(int argc, char **argv) {

}

#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main(void) {
	FILE *Linb = fopen("linb.txt", "r");

	if (!Linb) {
		fprintf(stderr, "Erro ao abrir o arquivo\n");
		exit(1);
	}

	unsigned char codigo[1000];
	funcp f = CompilaLinB(Linb, codigo);
	printf("Codigo na main: %d\n", (*f)(5, 2));

	fclose(Linb);
	return 0;
}

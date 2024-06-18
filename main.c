#include <stdio.h>
#include <stdlib.h>
#include "compilalinb.h"

int main(int argc, char *argv[]) {
	FILE *Linb = fopen(argv[1], "r");

	if (!Linb) {
		fprintf(stderr, "Erro ao abrir o arquivo\n");
		exit(1);
	}

	unsigned char codigo[1000];
	funcp f = CompilaLinB(Linb, codigo);
	printf("Codigo na main: %d\n", (*f)(atoi(argv[2]), atoi(argv[3])));

	fclose(Linb);
	return 0;
}

#include <stdio.h>

#include "histograma.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Uso: %s <nome do arquivo>\n", argv[0]);
		return 1;
	}
	
	leMetadados(argv[1]);

	printf("\n");
	return 0;
}

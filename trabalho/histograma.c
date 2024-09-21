#include <stdio.h>

#include "histograma.h"

void leMetadados(char *nomeDoArquivo) {
	FILE *imagem;
	char tipo;
	unsigned char pixel;
	
	imagem = fopen(nomeDoArquivo, "r");
	if(!imagem) {
		printf("Erro ao abrir a imagem\n");
		return;
	}
	
	while ( tipo != 'L' ) {
		fread(&tipo, sizeof(char), 1, imagem);
		printf("%c", tipo);
	}
	
	printf("\n");

	while (!feof(imagem)) {
		fread(&pixel, sizeof(unsigned char), 1, imagem);
		printf("%d ", pixel);
	}

	fclose(imagem);	
}

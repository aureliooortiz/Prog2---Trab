#include <stdio.h>
#include <stdlib.h>

#include "histograma.h"

void imprimeMatriz(int **imagem, int linhas, int colunas) {
	int i, j ;
	
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			printf("%d ", imagem[i][j]) ;
		}
		printf("\n") ;
	}
}

void liberaMatriz(struct imagem_t imagem) {
	int i ;
	
	for (i = 0; i < imagem.altura; i++) {
		free(imagem.matriz[i]) ;
	}
	
	free(imagem.matriz) ;
}


int main(int argc, char *argv[]) {
	struct imagem_t imagem ;
	struct imagem_t LBP ;
	int histograma[256] ;
	
	if (argc < 2) {
		printf("Uso: %s <nome do arquivo>\n", argv[0]) ;
		return 1;
	}
	
	imagem = criaMatrizDeImagem(argv[1]) ;
	if(!imagem.matriz) {
		printf("Erro ao criar matriz de imagem\n");
		return 1; 
	}
	
	LBP = criaMatrizLBP(imagem) ;
	
	imprimeMatriz(imagem.matriz, 10, 10) ;
	printf("\n") ;
	imprimeMatriz(LBP.matriz, 10, 10) ;
	
	criaHistograma(LBP, histograma);
	
	printf("\n") ;	
	
	for (int i = 0; i < 256; i++) {
		printf("%d ", histograma[i]);
	}
	
	printf("\n") ;
	
	criaImagemPGM("imagemLBP1.pgm", LBP) ;
	
	liberaMatriz(imagem) ;
	liberaMatriz(LBP) ;
	
	printf("\n") ;
	return 0 ;
}

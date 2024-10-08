#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#include "histograma.h"

// struct auxiliar para guardar nome e a posição no vetor distancias da imagem
struct imagensDiretorio {
	char nome[256] ;
	float histograma[256] ;
	float distancia ;
} ;

float distanciaCartesiana(float v1[], float v2[]) {
	float dist[256], soma ;
	int i ;
	
	soma = 0 ;
	for (i = 0; i < 256; i++) {
		dist[i] = v1[i] - v2[i] ;
		dist[i] *= dist[i] ;
		soma += dist[i] ;
	}
	
	return sqrt(soma) ;
}

int main(int argc, char *argv[]) {
	struct imagem_t imagem, LBP, LBP1, imagemDiretorio ;
	struct dirent *entrada ;
	struct imagensDiretorio *diretorio ;
	DIR *dir ;
	unsigned short int erro ;
	short int contI, contO, contD, caso1 ;
	float histograma1[256] ; 
	long n, contVet, i, j, menor ;
	char *nomeImagem, *saidaImagem, *nomeDiretorio, *concat ;
	
	if (argc < 5) {
		printf("Argumentos inválidos\n") ;
		return 1 ;
	}
	
	contI = 0 ;
	contO = 0 ;
	contD = 0 ;
	contVet = 0 ;
	i = 1 ;
	// Verifica se é para gerar imagem ou comparar com outras imagens
	caso1 = 0 ;
	while ( i < argc - 1) {
		// Retorna o tamanho da string
		n = strlen (argv[i+1]) ;
		if (strcmp(argv[i], "-i") == 0) {
			// Impede que a mesma flag se repita
			if (contI > 0) {
				printf("Argumentos inválidos\n") ;
				return 1 ;
			}
			// Aloca memória do tamanho do argumento
			nomeImagem = (char*)malloc(n+1) ;
			strcpy(nomeImagem, argv[i+1]);
			nomeImagem[n] = '\0' ;
			contI++ ;
		} else if (strcmp(argv[i], "-o") == 0) {
			if (contO > 0) {
				printf("Argumentos inválidos\n") ;
				return 1 ;
			}
			saidaImagem = (char*)malloc(n+1) ;
			strcpy(saidaImagem, argv[i+1]);
			saidaImagem[n] = '\0';
			caso1 = 1 ;
			contO++ ;
		} else if ((strcmp(argv[i], "-d") == 0)) {
			if (contD > 0) {
				printf("Argumentos inválidos\n") ;
				return 1 ;
			}
			dir = opendir(argv[i+1]) ;
			nomeDiretorio = (char*)malloc(n+1) ;
			strcpy(nomeDiretorio, argv[i+1]);
			nomeDiretorio[n] = '\0' ;
			if (!dir) { 
				printf ("Erro ao abrir diretório\n") ;
				return 1 ;
			}
			// Conta a quantidade de arquivos no diretorio
			while ((entrada = readdir(dir)) != NULL) {
				if ((entrada->d_type == DT_REG) ) {
					contVet++ ;
				}
			}
			// Cria um vetor com essa quantidade para o calculo da distancia
			// cartesiana para cada uma das imagens
			diretorio = malloc(contVet * sizeof(struct imagensDiretorio)) ;
			if (!diretorio) {
				printf("Erro ao criar vetor de distancias\n") ;
				return 1 ;
			}		
			// Volta ao início do diretório
			rewinddir(dir) ;	
			
			contD++ ;
		} else {
			printf ("Argumentos inválidos\n") ;
			return 1 ;
		}
		i += 2 ;
	}
		
	erro = criaMatrizDeImagem(nomeImagem, &imagem) ;
	if (erro) {
		printf("Erro ao criar matriz de imagem\n") ;
		return 1 ; 
	}
	
	criaMatrizLBP(imagem, &LBP) ;
	
	if (caso1) {
		criaImagemPGM(saidaImagem, LBP) ;
		free(saidaImagem) ;
	} else {
		i = 0 ;
		criaHistograma(LBP, histograma1) ;
		while ((entrada = readdir(dir)) != NULL) {
			if ((entrada->d_type == DT_REG)) {	
				
				n = strlen (entrada->d_name) ;
				j = strlen (nomeDiretorio) ;
				// Concatena nome do arquivo e do diretorio para ser possivel abrir
				concat = (char*)malloc((n+j+1)) ;
				strcpy(concat, nomeDiretorio) ;
				strcat(concat, entrada->d_name) ;
				concat[n+j] = '\0' ;
				
				erro = criaMatrizDeImagem(concat, &imagemDiretorio) ; 
				if (erro) {
					contVet-- ;
					free(concat) ;
					
					continue ;
				}
				criaMatrizLBP(imagemDiretorio, &LBP1) ;
				criaHistograma(LBP1, diretorio[i].histograma) ;
				strcpy(diretorio[i].nome, entrada->d_name) ;
				diretorio[i].distancia = distanciaCartesiana(histograma1, diretorio[i].histograma) ;
				
				liberaMatriz(imagemDiretorio) ;
				liberaMatriz(LBP1) ;
				free(concat) ;	
				i++ ;
			}	
		}
		menor = 0;
		for (j = 0; j < i; j++) {
			if(diretorio[menor].distancia > diretorio[j].distancia){
				menor = j;
			}
		}
		printf("Imagem mais similar: %s %.6f\n", diretorio[menor].nome, diretorio[menor].distancia) ;
		
		free(nomeDiretorio) ;
		free(diretorio) ;
		closedir(dir) ;
	}
	free(nomeImagem) ;
	liberaMatriz(imagem) ;
	liberaMatriz(LBP) ;
	
	return 0 ;
}

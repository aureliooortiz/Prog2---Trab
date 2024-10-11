#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#include "histograma.h"

// struct auxiliar para guardar nome e a posição no vetor distancias da imagem
struct imagensDiretorio {
	// Um arquivo tem no máximo 255 caracteres
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
	unsigned short int contI, contO, contD, caso1, erro, o, d, libera ;
	float histograma1[256] ; 
	long n, contVet, i, j, menor ;
	char *nomeImagem, *saidaImagem, *nomeDiretorio, *concat ;
	
	if (argc < 5) {
		printf("Argumentos inválidos\n") ;
		return 1 ;
	}
	
	nomeImagem = NULL ; 
	saidaImagem = NULL ; 
	nomeDiretorio = NULL ;
	concat = NULL ;
	diretorio = NULL ;
	entrada = NULL ;
	dir = NULL ;
	
	contI = 0 ;
	contO = 0 ;
	contD = 0 ;
	contVet = 0 ;
	// Diz se as matrizes foram alocadas dinamicamente
	libera = 0 ;
	i = 1 ;
	// Impede que chamemos usemos as flags -d e -o juntos
	d = 0 ;
	o = 0 ;
	// Verifica se é para gerar imagem(caso1) ou comparar com outras imagens(caso2)
	caso1 = 0 ;
	while ( i < argc - 1) {
		// Retorna o tamanho da string
		n = strlen (argv[i+1]) ;
		if (strcmp(argv[i], "-i") == 0) {
			// Impede que a mesma flag se repita
			if (contI > 0) {
				printf("Argumentos inválidos\n") ;
				if (saidaImagem != NULL) {
					free(saidaImagem) ;
				}
				if (nomeDiretorio != NULL) {
					free(nomeDiretorio) ;
				}				
				if (dir != NULL) {
					closedir(dir) ;
				}
				if (diretorio != NULL) {
					free(diretorio) ;
				}				
								
				return 1 ;
			}
			// Aloca memória do tamanho do argumento
			nomeImagem = (char*)malloc(n+1) ;
			if (!nomeImagem) {
				printf("Erro ao alocar memória\n") ;
				if (saidaImagem != NULL) {
					free(saidaImagem) ;
				}
				if (nomeDiretorio != NULL) {
					free(nomeDiretorio) ;
				}				
				if (dir != NULL) {
					closedir(dir) ;
				}
				if (diretorio != NULL) {
					free(diretorio) ;
				}	
				return 1 ;
			}
			strcpy(nomeImagem, argv[i+1]);
			nomeImagem[n] = '\0' ;
			contI++ ;
		} else if ((strcmp(argv[i], "-o") == 0) && (!d)) {
			if (contO > 0) {
				printf("Argumentos inválidos\n") ;
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}		
				
				return 1 ;
			}
			saidaImagem = (char*)malloc(n+1) ;
			if (!saidaImagem) {
				printf("Erro ao alocar memória\n") ;
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}				
				return 1 ;
			}
			strcpy(saidaImagem, argv[i+1]);
			saidaImagem[n] = '\0';
			caso1 = 1 ;
			o = 1 ;
			contO++ ;
		} else if (((strcmp(argv[i], "-d") == 0)) && (!o)) {
			if (contD > 0) {
				printf("Argumentos inválidos\n") ;
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}
				
				return 1 ;
			}
			dir = opendir(argv[i+1]) ;
			if (!dir) { 
				printf ("Erro ao abrir diretório\n") ;
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}
				
				return 1 ;
			}
			nomeDiretorio = (char*)malloc(n+1) ;
			if (!nomeDiretorio) {
				printf("Erro ao alocar memória\n") ;
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}
				closedir(dir) ;
				
				return 1 ;
			}
			strcpy(nomeDiretorio, argv[i+1]);
			nomeDiretorio[n] = '\0' ;
			d = 1 ;
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
				if (nomeImagem != NULL) {
					free(nomeImagem) ;
				}
				free(nomeDiretorio) ;			
				closedir(dir) ;			
				
				return 1 ;
			}		
			// Volta ao início do diretório
			rewinddir(dir) ;	
			
			contD++ ;
		} else {
			printf ("Argumentos inválidos\n") ;
			if (nomeImagem != NULL) {
				free(nomeImagem) ;
			}
			if (saidaImagem != NULL) {
				free(saidaImagem) ;
			}			
			if (nomeDiretorio != NULL) {
				free(nomeDiretorio) ;
			}				
			if (dir != NULL) {
				closedir(dir) ;
			}
			if (diretorio != NULL) {
				free(diretorio) ;
			}			
			
			return 1 ;
		}
		i += 2 ;
	}
	
	// Entra apenas se estiver no caso1, ou não tiver vetor histograma e estiver no caso2
	if (caso1 || (!(procuraVetorHistograma(nomeImagem, nomeDiretorio)) && !caso1)) {	
		libera = 1 ;
		erro = criaMatrizDeImagem(nomeImagem, &imagem) ;
		if (erro) {
			printf("Erro ao criar matriz de imagem\n") ;
			free(nomeImagem) ;
			if (saidaImagem != NULL) {
				free(saidaImagem) ;
			}			
			if (nomeDiretorio != NULL) {
				free(nomeDiretorio) ;
			}				
			if (dir != NULL) {
				closedir(dir) ;
			}
			if (diretorio != NULL) {
				free(diretorio) ;
			}	
			return 1 ; 
		}
		
		if (!(criaMatrizLBP(imagem, &LBP))) {
			printf("Erro ao criar matriz de LBP\n") ;
			free(nomeImagem) ;
			if (saidaImagem != NULL) {
				free(saidaImagem) ;
			}			
			if (nomeDiretorio != NULL) {
				free(nomeDiretorio) ;
			}				
			if (dir != NULL) {
				closedir(dir) ;
			}
			if (diretorio != NULL) {
				free(diretorio) ;
			}			
			return 1 ;
		}
		// Indica que não há histograma da imagem 
		erro = -2 ;
	}	
		
	if (caso1) {
		if (!(criaImagemPGM(saidaImagem, LBP) )) {
			printf("Erro ao criar arquivo PGM\n") ;
			free(nomeImagem) ;
			free(saidaImagem) ;
			liberaMatriz(imagem) ;
			liberaMatriz(LBP) ;
			
			return 1 ;
		}
		free(saidaImagem) ;
	// Caso2	
	} else {
		i = 0 ;
		if (erro == -2) {
			if (!(criaHistograma(LBP, histograma1))) {
				printf("Erro ao criar histograma1\n") ;
				free(nomeImagem) ;
				liberaMatriz(imagem) ;
				liberaMatriz(LBP) ;
				free(nomeDiretorio) ;
				free(diretorio) ;
				closedir(dir) ;
				
				return 1 ;
			}
			// Verificar erros de armazenaVetor e leVetorLBP
			armazenaVetor(histograma1, nomeImagem, nomeDiretorio) ;
		} else {
			leVetorLBP(nomeImagem, nomeDiretorio, histograma1) ;
		}	
		while ((entrada = readdir(dir)) != NULL) {
			if ((entrada->d_type == DT_REG)) {	
				// Verifica se o vetor histograma já existe
				if (!(procuraVetorHistograma(entrada->d_name, nomeDiretorio))) {	
					n = strlen (entrada->d_name) ;
					j = strlen (nomeDiretorio) ;
					// Concatena nome do arquivo e do diretorio para ser possivel abrir
					concat = (char*)malloc((n+j+1)) ;
					concat[n+j] = '\0' ;
					strcpy(concat, nomeDiretorio) ;
					strcat(concat, entrada->d_name) ;
					
					erro = criaMatrizDeImagem(concat, &imagemDiretorio) ; 
					if (erro) {
						contVet-- ;
						free(concat) ;
						
						continue ;
					}
					strcpy(diretorio[i].nome, entrada->d_name) ;
					if(!(criaMatrizLBP(imagemDiretorio, &LBP1))) {
						printf("Erro ao criar matriz LBP\n") ;
						free(nomeImagem) ;
						if (libera) {
							liberaMatriz(imagem) ; 
							liberaMatriz(LBP) ;
						}	
						free(nomeDiretorio) ;
						free(diretorio) ;
						closedir(dir) ;
					
						return 1 ;
					}
					if(!(criaHistograma(LBP1, diretorio[i].histograma))) {
						printf("Erro ao criar histograma2\n") ;
						free(nomeImagem) ;
						if (libera) {
							liberaMatriz(imagem) ; 
							liberaMatriz(LBP) ;
						}	
						free(nomeDiretorio) ;
						free(diretorio) ;
						closedir(dir) ;
					
						return 1 ;
					}
					armazenaVetor(diretorio[i].histograma, entrada->d_name, nomeDiretorio) ;
					diretorio[i].distancia = distanciaCartesiana(histograma1, diretorio[i].histograma) ;
					
					liberaMatriz(imagemDiretorio) ; 
					liberaMatriz(LBP1) ;
					free(concat) ;	
					i++ ;
				} else {
					strcpy(diretorio[i].nome, entrada->d_name) ;
					leVetorLBP(entrada->d_name, nomeDiretorio, diretorio[i].histograma) ;
					diretorio[i].distancia = distanciaCartesiana(histograma1, diretorio[i].histograma) ;
					i++ ;
				}
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
	if (libera) {	
		liberaMatriz(imagem) ;
		liberaMatriz(LBP) ;
	}
	
	return 0 ;
}

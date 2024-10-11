#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <ctype.h>

#include "histograma.h"

void liberaMatriz(struct imagem_t imagem) {
	int i ;
	
	if (!imagem.matriz) {
		printf("Imagem não inicializada\n") ;
		return ;
	}
	
	for (i = 0; i < imagem.altura; i++) {
		free(imagem.matriz[i]) ;
	}
	
	free(imagem.matriz) ;
}

// Retorna em im uma matriz de uma imagem, 
// e retorna 1 caso a imagem não seja um PGM ou algo dê errado e 0 caso contrário
unsigned short int criaMatrizDeImagem(char *nomeDoArquivo, struct imagem_t *im) {
	FILE *imagem ;
	char comentarios ;
	unsigned char pixel ;
	int pix, i, j, x, y ;
	
	imagem = fopen(nomeDoArquivo, "rb") ;
	if(!imagem) {
		perror("Erro ao abrir o arquivo") ; 
		return 1 ;
	}
	
	// Leitura do cabeçalho
	if ((fscanf(imagem, "%2s", im->tipo)) != 1) {
		printf("Tipo de arquivo não suportado-1\n") ;
		fclose(imagem) ;
		return 1 ;
	}
	im->tipo[2] = '\0';
	if ((strcmp(im->tipo, "P2") != 0) && (strcmp(im->tipo, "P5") != 0)) {
		printf("Tipo de arquivo não suportado0\n") ;
		fclose(imagem) ;
		return 1 ;
	}
	
	// Leitura dos comentários
	do {
		comentarios = fgetc(imagem);
		// Se encontrar um comentário, ignora a linha inteira
		if (comentarios == '#') {
			// Pular até o fim da linha
			while (fgetc(imagem) != '\n') ;  
		}
	// Continua ignorando até encontrar algo que não seja espaço ou comentário	
	} while (comentarios == '#' || isspace(comentarios)) ;  

	// Como já leu um caractere, precisamos colocá-lo de volta no stream
	ungetc(comentarios, imagem) ;
	
	if ((fscanf(imagem, "%d %d", &(im->largura), &(im->altura))) != 2) {
		printf("Tipo de arquivo não suportado1\n") ;
		fclose(imagem);
		return 1;
	}
	if ((im->largura < 0) || (im->altura) < 0) {
		printf("Tipo de arquivo não suportado2\n") ;
		fclose(imagem);
		return 1 ;
	}
	
	if ((fscanf(imagem, "%d", &(im->maxValor))) != 1) {
		printf("Tipo de arquivo não suportado3\n") ;
		fclose(imagem);
		return 1;
	}	
	if (im->maxValor < 0) {
		printf("Tipo de arquivo não suportado4\n") ;
		fclose(imagem);
		return 1 ;
	}
	
	// Ignorar o caractere de nova linha que pode ficar no buffer
	fgetc(imagem) ;	
	
	// Aloca memória para um vetor de ponteiros (linhas)
	im->matriz = (int **)malloc( (im->altura) * sizeof(int *) ) ;
	if(!(im->matriz)) {
		printf("Erro ao alocar para matriz\n") ;
		fclose(imagem) ;
		return 1 ;
	}
	
	// Aloca memória para cada linha na matriz
	for (i = 0; i < (im->altura); i++) {
		
		im->matriz[i] = (int *)malloc( (im->largura) * sizeof(int)) ;
		
		// Libera memória alocada caso de errado
		if( !(im->matriz[i]) ) {
			for (j = 0; j < i; j++) {
				free(im->matriz[j]) ;
			}
			free(im->matriz) ;
			im->matriz = NULL ;
			fclose(imagem) ;
			printf("Erro ao alocar para matriz\n") ;
			
			return 1 ;
			
		}
	}
	
	if (strcmp(im->tipo, "P2") == 0) {
		// Leitura dos dados de arquivo P2
		for (y = 0; y < (im->altura); y++) {
			for (x = 0; x < (im->largura); x++) {
				fscanf(imagem, "%d", &pix) ;
				im->matriz[y][x] = pix ; 
				// Verifica se o pixel é válido
				if ((im->matriz[y][x] > im->maxValor) || (im->matriz[y][x] < 0)) {
					printf("Tipo de arquivo não suportado\n") ;
					liberaMatriz(*im) ;	
					im->matriz = NULL ;	
					fclose(imagem) ;
					
					return 1 ;
				}
			}
		}
	} else {
		// Leitura dos dados dos pixels para P5
		for (y = 0; y < (im->altura); y++) {
			for (x = 0; x < (im->largura); x++) {
				fread(&pixel, sizeof(unsigned char), 1, imagem) ;
				// Armazenar como inteiro na matriz
				im->matriz[y][x] = (int)pixel ; 
				// Verifica se o pixel é válido
				if ((im->matriz[y][x] > im->maxValor) || (im->matriz[y][x] < 0)) {
					printf("Tipo de arquivo não suportado\n") ;
					liberaMatriz(*im) ;	
					im->matriz = NULL ;	
					fclose(imagem) ;
					
					return 1 ;
				}
			}
		}
	}
	
	fclose(imagem);	
	
	return 0 ;
}

unsigned short int criaMatrizLBP(struct imagem_t imagem, struct imagem_t *LBP) {
	// Contador de linhas e colunas na matriz imagem
	int lin, col ;
	// Contador de linhas e colunas da matriz bitmap
	int mC, mL ;
	int i, j ;
	int soma;
	int bitmap[3][3] ;
	int pixel[3][3] ;
	
	// Inicia a matriz mascara
	int mascara[3][3] = {
		{1, 2, 4} ,
		{8, 0 , 16} ,
		{32, 64, 128}
	} ;
	
	// O cálculo do LBP só funciona para imagens que 
	// tenham até 256 níveis de cinza cada pixel
	if (!imagem.matriz) {
		printf("Imagem não inicializada\n") ;
		return 0 ;
	}
	
	// Aloca memória para a matriz LBP 
	LBP->matriz = (int **)malloc( (imagem.altura) * sizeof(int *) ) ;
	if(!(LBP->matriz)) {
		printf("Erro ao alocar para matriz\n") ;
		return 0 ;
	}
	
	// Aloca memória para cada linha na matriz LBP
	for (i = 0; i < (imagem.altura); i++) {
		LBP->matriz[i] = (int *)malloc( (imagem.largura) * sizeof(int)) ;
		
		// Libera memória alocada caso de errado
		if( !(LBP->matriz[i]) ) {
			for (j = 0; j < i; j++) {
				free(LBP->matriz[j]) ;
			}
			free(LBP->matriz) ;
			LBP->matriz = NULL ;
			printf("Erro ao alocar para matriz\n") ;
			
			return 0 ;
		}
	}
	
	LBP->altura = imagem.altura ;
	LBP->largura = imagem.largura ;
	strcpy(LBP->tipo, imagem.tipo) ;
	LBP->maxValor = imagem.maxValor ;
	
	// Copia a primeira linha para LBP
	for (j = 0; j < imagem.largura; j++) {
		LBP->matriz[0][j] = imagem.matriz[0][j] ; 
	}
	// Copia a ultima linha para LBP
	for (j = 0; j < imagem.largura; j++) {
		LBP->matriz[imagem.altura-1][j] = imagem.matriz[imagem.altura-1][j] ; 
	}
	// Copia a primeira coluna para LBP
	for (i = 0; i < imagem.altura; i++) {
		LBP->matriz[i][0] = imagem.matriz[i][0] ; 
	}
	// Copia a ultima coluna para LBP
	for (i = 0; i < imagem.altura; i++) {
		LBP->matriz[i][imagem.largura-1] = imagem.matriz[i][imagem.largura-1] ; 
	}
	
	lin = 0 ;
	// Passa por todas as linhas da matriz imagem para criar a matriz bitmap
	while (lin+2 < imagem.altura) {
		col = 0 ;
		// Passa por todas as colunas na linha 
		while (col+2 < imagem.largura) {
			mL = 0 ;
			// Cria a matriz bitmap 3x3
			for (i = lin; i <= lin+2; i++) {
				mC = 0;
				for (j = col; j <= col+2; j++) {
					if (imagem.matriz[lin+1][col+1] > imagem.matriz[i][j]) {
						bitmap[mL][mC] = 0 ;	
					} else {
						bitmap[mL][mC] = 1 ;
					}
					mC++ ;
				}
				mL++ ;
			}
			
			// Mulitplica bitmap x mascara e armazena na matriz pixel
			for (i = 0; i < 3; i ++) {
				for (j = 0; j < 3; j++) {
					pixel[i][j] = bitmap[i][j] * mascara[i][j] ;
				}
			}
			
			// Faz a soma dos elementos de pixel
			soma = 0 ;
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 3; j++) {
					soma += pixel[i][j];
				}
			}
			
			LBP->matriz[lin+1][col+1] = soma;	
				
			col++ ; 
		}
		lin++ ;
	}
	
	return 1 ;
}

// Normaliza o nome para imagem.lbp
char *normalizaNome(const char *nomeImagem, const char *nomeDiretorio) {
    char *busca, *nomeNormalizado;
    int nomeLen, extensaoLen;

    // Aloca uma nova string para armazenar o nome normalizado
    nomeNormalizado = (char*)malloc(strlen(nomeImagem) + 5); 
    if (nomeNormalizado == NULL) {
        printf("Falha na alocação de memória.\n");
        return NULL;
    }

    strcpy(nomeNormalizado, nomeImagem);

    // Mantem apenas o nome da imagem e não o caminho, caso o nome seja o caminho
    busca = strstr(nomeNormalizado, nomeDiretorio);
    if (busca != NULL) {
        busca += strlen(nomeDiretorio);
        memmove(nomeNormalizado, busca, strlen(busca) + 1);  
    }

    // Verifica a extensão da imagem
    busca = strrchr(nomeNormalizado, '.');
    if (!busca) {
        printf("Imagem sem extensão\n");
        free(nomeNormalizado);  
        return NULL;
    }

	// Verifica se a string completa, incluindo a nova extensão, excede 255 caracteres
    nomeLen = strlen(nomeNormalizado);
    extensaoLen = strlen(busca);
    if (nomeLen - extensaoLen + 4 > 255) {
        printf("Sem espaço para adicionar extensão\n");
        free(nomeNormalizado);  
        return NULL;
    }

    strcpy(busca + 1, "lbp");

    return nomeNormalizado;
}

// Lê o arquivo e armazena seus valores em vet
unsigned short int leVetorLBP(char *nomeImagem, char *nomeDiretorio, float vet[]) {
	char *nomeNormalizado ;	
	FILE *arq ;
	
	nomeNormalizado = normalizaNome(nomeImagem, nomeDiretorio) ;
	
	// Coloca os valores do arquivo em um vetor
	arq = fopen(nomeNormalizado, "rb") ;
	if (!arq) {
		printf("Erro ao abrir arquivo\n") ;
		return 0 ;
	}
	fread(vet, sizeof(float), 256, arq) ;
	
	if (nomeNormalizado != NULL) {
		free(nomeNormalizado) ;
	}
	fclose(arq) ;

	return 1 ;
}

// Procura no diretorio atual o arquivo .lbp, retorna 1 se existe e 0 caso não
// e retorna -1 em caso de erro
unsigned short int procuraVetorHistograma(char *nomeImagem, char *nomeDiretorio) {
	char *nomeNormalizado ;
	struct dirent *entrada ;
	DIR *dir ;	
		
	nomeNormalizado = normalizaNome(nomeImagem, nomeDiretorio) ;
	
	dir = opendir(".") ;
	if (!dir) { 
		printf ("Erro ao abrir diretório\n") ;
		return -1 ;
	}
	
	// Percorre o diretório procurando um arquivo com o nomeDaImagem.lbp
	while ((entrada = readdir(dir)) != NULL ) {
		if ((entrada->d_type == DT_REG)) {
			if ((strcmp(entrada->d_name, nomeNormalizado)) == 0) {
				if (nomeNormalizado != NULL) {
					free(nomeNormalizado) ;
				}	
				closedir(dir) ;	
				
				return 1 ;
			}
		}
	}
	
	if (nomeNormalizado != NULL) {
		free(nomeNormalizado) ;
	}	
	closedir(dir) ;
	
	return 0 ;
}

// Cria um arquivo no diretório atual contendo o vetor histograma da imagem
unsigned short int armazenaVetor(float vetor[], char *nomeImagem, char *nomeDiretorio) {
	char *nomeNormalizado ;
	FILE *arq ;
	
	nomeNormalizado = normalizaNome(nomeImagem, nomeDiretorio) ;
	
	if (!nomeNormalizado) {
		printf("Erro ao normalizar nome\n") ;
		return 0 ;
	}
	
	// Cria arquivo binário no diretorio atual e armazena o vetor
	arq = fopen(nomeNormalizado, "wb") ;
	if (!arq) {
		printf("Erro ao abrir arquivo\n") ;
		return 0 ;
	}

	fwrite(vetor, sizeof(float), 256, arq) ;
	
	free(nomeNormalizado) ;		
	fclose(arq);
	
	return 1 ;	
}

// Cria o vetor histograma da matriz LBP, e salva em um arquivo
unsigned short int criaHistograma(struct imagem_t LBP, float vetor[]) {
	int i, j ;
	float magnitude ;
	
	if (!LBP.matriz) {
		printf("Imagem não inicializada\n") ;
		return 0 ;
	}
	
	// Preenche vetor com 0
	for (i = 0; i < 256; i++) {
		vetor[i] = 0 ;
	}
	// Cria histograma
	for (i = 0; i < LBP.altura; i++) {
		for (j = 0; j < LBP.largura; j++) {
			vetor[ LBP.matriz[i][j] ]++;
		}
	}
	
	magnitude = 0 ;
	// Normaliza o vetor
	for (i = 0; i < 256; i++) {
		magnitude += vetor[i] ;
	}
	magnitude = sqrt(magnitude) ;
	for (i = 0; i < 256; i++) {
		vetor[i] /= magnitude ;	
	}
	
	return 1 ;
}

// Cria uma imagem PGM a partir de uma matriz LBP
unsigned short int criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) {
	FILE *arquivo ;
	int i, j, pix ;
	unsigned char pixel; 
			
	if (!LBP.matriz) {
		printf("Imagem não inicializada\n") ;
		return 0 ;
	}
	
	// Não sobreescreve o arquivo caso ele exista e retorna
	arquivo = fopen(nomeDoArquivo, "wx") ;
	if (!arquivo) {
		printf("Erro ao abrir a imagem\n") ;
		return 0 ;
	}
    
	// Escreve o cabeçalho PGM (P5 ou P2, largura, altura, valor máximo)
	fprintf(arquivo, "%s\n", LBP.tipo) ;
	fprintf(arquivo, "%d %d\n", LBP.largura, LBP.altura) ;
	fprintf(arquivo, "%d\n", LBP.maxValor) ;  
    
    if (strcmp(LBP.tipo, "P5") == 0) {
		// Escreve os dados da matriz em formato binário
		for (i = 0; i < LBP.altura; i++) {
			for (j = 0; j < LBP.largura; j++) {
				pixel = (unsigned char)LBP.matriz[i][j];  
				fwrite(&pixel, sizeof(unsigned char), 1, arquivo);   
			}
		}
	} else {
		for (i = 0; i < LBP.altura; i++) {
			for (j = 0; j < LBP.largura; j++) {
				pix = LBP.matriz[i][j] ; 
				if (j == LBP.largura-1) {
					fprintf(arquivo, "%d\n", pix) ;	
				} else {
					fprintf(arquivo, "%d ", pix) ;   
				}	
			}
		}
	}
	fclose(arquivo) ;
	
	return 1 ;
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "histograma.h"

// Retorna uma struct imagem_t com informações do arquivo da imagem
struct imagem_t criaMatrizDeImagem(char *nomeDoArquivo) {
	FILE *imagem ;
	char tipo[3] ;
	char comentarios ;
	unsigned char pixel ;
	int maxValor, i, j, x, y ;
	struct imagem_t im ;
	
	// Inicializa a estrutura com valores padrão
    im.matriz = NULL;
    im.largura = 0;
    im.altura = 0;
	
	imagem = fopen(nomeDoArquivo, "r") ;
	if(!imagem) {
		printf("Erro ao abrir a imagem\n") ; 
		return im ;
	}
	
	// Leitura do cabeçalho
	fscanf(imagem, "%2s", tipo) ;
	
	// Leitura dos comentários
	do {
		comentarios = fgetc(imagem);
		// Se encontrar um comentário, ignora a linha inteira
		if (comentarios == '#') {
			// Pular até o fim da linha
			while (fgetc(imagem) != '\n');  
		}
	// Continua ignorando até encontrar algo que não seja espaço ou comentário	
	} while (comentarios == '#' || isspace(comentarios));  

	// Como já leu um caractere, precisamos colocá-lo de volta no stream
	ungetc(comentarios, imagem);
	
	fscanf(imagem, "%d %d", &(im.largura), &(im.altura)) ;
	fscanf(imagem, "%d", &maxValor) ;
	
	// Ignorar o caractere de nova linha que pode ficar no buffer
	fgetc(imagem) ;	
	
	tipo[2] = '\0';
	
	// Imprime o cabeçalho
	printf("%s\n", tipo) ;
	printf("%d %d\n", im.largura, im.altura) ;
	printf("%d\n", maxValor) ;
	
	// Aloca memória para um vetor de ponteiros (linhas)
	im.matriz = (int **)malloc( (im.altura) * sizeof(int *) );
	if(!(im.matriz)) {
		printf("Erro ao alocar para matriz\n");
		fclose(imagem);
		return im;
	}
	
	// Aloca memória para cada linha na matriz
	for (i = 0; i < (im.altura); i++) {
		
		im.matriz[i] = (int *)malloc( (im.largura) * sizeof(int));
		
		// Libera memória alocada caso de errado
		if( !(im.matriz[i]) ) {
			for (j = 0; j < i; j++) {
				free(im.matriz[j]);
			}
			free(im.matriz);
			im.matriz = NULL;
			fclose(imagem);
			printf("Erro ao alocar para matriz\n");
			
			return im;
			
		}
	}
	
	// Leitura dos dados dos pixels
	for (y = 0; y < (im.altura); y++) {
		for (x = 0; x < (im.largura); x++) {
			fread(&pixel, sizeof(unsigned char), 1, imagem);
			// Armazenar como inteiro na matriz
			im.matriz[y][x] = (int)pixel; 
		}
	}

	fclose(imagem);	
	
	return im;
}

// Retorna a struct com matriz LBP da imagem
struct imagem_t criaMatrizLBP(struct imagem_t imagem) {
	// Contador de linhas e colunas na matriz imagem
	int lin, col ;
	// Contador de linhas e colunas da matriz bitmap
	int mC, mL ;
	int i, j ;
	int soma;
	int bitmap[3][3] ;
	int pixel[3][3] ;
	struct imagem_t LBP ;
	
	// Inicia a matriz mascara
	int mascara[3][3] = {
		{1, 2, 4} ,
		{8, 0 , 16} ,
		{32, 64, 128}
	} ;
	
	// Aloca memória para a matriz LBP 
	LBP.matriz = (int **)malloc( (imagem.altura) * sizeof(int *) ) ;
	if(!(LBP.matriz)) {
		printf("Erro ao alocar para matriz\n") ;
		return LBP ;
	}
	
	// Aloca memória para cada linha na matriz LBP
	for (i = 0; i < (imagem.altura); i++) {
		LBP.matriz[i] = (int *)malloc( (imagem.largura) * sizeof(int)) ;
		
		// Libera memória alocada caso de errado
		if( !(LBP.matriz[i]) ) {
			for (j = 0; j < i; j++) {
				free(LBP.matriz[j]) ;
			}
			free(LBP.matriz) ;
			LBP.matriz = NULL ;
			printf("Erro ao alocar para matriz\n") ;
			
			return LBP ;
		}
	}
	
	LBP.altura = imagem.altura ;
	LBP.largura = imagem.largura ;
	
	// Copia a primeira linha para LBP
	for (j = 0; j < imagem.largura; j++) {
		LBP.matriz[0][j] = imagem.matriz[0][j] ; 
	}
	// Copia a ultima linha para LBP
	for (j = 0; j < imagem.largura; j++) {
		LBP.matriz[imagem.altura-1][j] = imagem.matriz[imagem.altura-1][j] ; 
	}
	// Copia a primeira coluna para LBP
	for (i = 0; i < imagem.altura; i++) {
		LBP.matriz[i][0] = imagem.matriz[i][0] ; 
	}
	// Copia a ultima coluna para LBP
	for (i = 0; i < imagem.altura; i++) {
		LBP.matriz[i][imagem.largura-1] = imagem.matriz[i][imagem.largura-1] ; 
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
			
			LBP.matriz[lin+1][col+1] = soma;	
				
			col++ ; 
		}
		lin++ ;
	}
	
	return LBP;
}

// Cria o vetor histograma da matriz LBP
void criaHistograma(struct imagem_t LBP, int vetor[]) {
	int i, j ;
	
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
}

// Cria uma imagem PGM a partir de uma matriz LBP
void criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) {
	FILE *arquivo;
	int i, j;
	unsigned char pixel;
    
	arquivo = fopen(nomeDoArquivo, "w");
	if (!arquivo) {
		printf("Erro ao abrir a imagem\n");
		return;
	}
    
	// Escreve o cabeçalho PGM (P5, largura, altura, valor máximo)
	fprintf(arquivo, "P5\n");
	fprintf(arquivo, "%d %d\n", LBP.largura, LBP.altura);
	fprintf(arquivo, "255\n");  
    
	// Escreve os dados da matriz em formato binário
	for (i = 0; i < LBP.altura; i++) {
		for (j = 0; j < LBP.largura; j++) {
			pixel = (unsigned char)LBP.matriz[i][j];  // Converte para byte
			fwrite(&pixel, sizeof(unsigned char), 1, arquivo);   // Escreve cada pixel
		}
	}

}

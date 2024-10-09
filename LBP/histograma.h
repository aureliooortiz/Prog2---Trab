#ifndef __HISTOGRAMA__
#define __HISTOGRAMA__

struct imagem_t {
	int **matriz ;
	int largura ;
	int altura ;
	int maxValor ;
	char tipo[3] ;
} ;

// Libera memória alocada da matriz
void liberaMatriz(struct imagem_t imagem) ;

// Pega os pixels da imagem e os coloca em uma matriz
unsigned short int criaMatrizDeImagem(char *nomeDoArquivo, struct imagem_t *im) ;

// Faz o tratamento dos pixels e transforma a matriz imagem em matriz LBP
unsigned short int criaMatrizLBP(struct imagem_t imagem, struct imagem_t *LBP) ;

// Cria o vetor histograma da matriz LBP
short int criaHistograma(struct imagem_t LBP, float vetor[], char *nomeImagem,
									char *nomeDiretorio) ;

// Cria uma imagem PGM a partir de uma matriz LBP
unsigned short int criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) ;

#endif

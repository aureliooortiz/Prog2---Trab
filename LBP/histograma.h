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

// Lê o arquivo e armazena seus valores em vet
void leVetorLBP(char *nomeImagem, char *nomeDiretorio, float vet[]) ;

// Cria um arquivo no diretório atual contendo o vetor histograma da imagem
void armazenaVetor(float vetor[], char *nomeImagem, char *nomeDiretorio) ;

// Procura no diretorio atual o arquivo .lbp, retorna 1 se existe e 0 caso não
unsigned short int procuraVetorHistograma(char *nomeImagem, char *nomeDiretorio) ;

// Cria o vetor histograma da matriz LBP
unsigned short int criaHistograma(struct imagem_t LBP, float vetor[]) ;

// Cria uma imagem PGM a partir de uma matriz LBP
unsigned short int criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) ;

#endif

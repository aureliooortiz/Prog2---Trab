#ifndef __HISTOGRAMA__
#define __HISTOGRAMA__

struct imagem_t {
	int **matriz ;
	int largura ;
	int altura ;
} ;

void liberaMatriz(struct imagem_t imagem) ;

unsigned short int criaMatrizDeImagem(char *nomeDoArquivo, struct imagem_t *im) ;

void criaMatrizLBP(struct imagem_t imagem, struct imagem_t *LBP) ;

// Cria o vetor histograma da matriz LBP
void criaHistograma(struct imagem_t LBP, float vetor[]) ;

// Cria uma imagem PGM a partir de uma matriz LBP
void criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) ;

#endif

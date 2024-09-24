#ifndef __HISTOGRAMA__
#define __HISTOGRAMA__

struct imagem_t {
	int **matriz ;
	int largura ;
	int altura ;
} ;

// Retorna uma struct imagem_t com informações do arquivo da imagem
struct imagem_t criaMatrizDeImagem(char *nomeDoArquivo) ;

// Retorna a struct com matriz LBP da imagem
struct imagem_t criaMatrizLBP(struct imagem_t imagem) ;

// Cria o vetor histograma da matriz LBP
void criaHistograma(struct imagem_t LBP, int vetor[]) ;

// Cria uma imagem PGM a partir de uma matriz LBP
void criaImagemPGM(char *nomeDoArquivo, struct imagem_t LBP) ;

#endif

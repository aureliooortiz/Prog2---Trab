# Nome do arquivo de saída
OUTPUT = programa

# Compilador
CC = gcc

# Opções de compilação
CFLAGS = -Wall -g -Wextra 

# Lista de arquivos fonte
SRCS = histograma.c main.c

# Arquivos objetos gerados a partir dos arquivos fonte
OBJS = $(SRCS:.c=.o)

# Comando para compilar
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS)

# Comando para limpar os arquivos gerados pela compilação
clean:
	rm -f $(OBJS) $(OUTPUT)

# Dependências
jogo.o: histograma.c histograma.h
main.o: main.c histograma.h

# Comando para executar o jogo
run: $(OUTPUT)
	./$(OUTPUT)


# Nome dos compiladores
CC = g++

# Flags de compilação
CFLAGS = -Wall

# Nomes dos arquivos executáveis
CLIENTE = cliente
SERVIDOR = servidor
EXIBIDOR = exibidor

# Comandos para criar os executáveis
all: $(SERVIDOR) $(CLIENTE) $(EXIBIDOR)

$(SERVIDOR): servidor.cpp
	$(CC) $(CFLAGS) -o $(SERVIDOR) servidor.cpp -pthread

$(CLIENTE): cliente.cpp
	$(CC) $(CFLAGS) -o $(CLIENTE) cliente.cpp -pthread

$(EXIBIDOR): cliente_exibicao.cpp
	$(CC) $(CFLAGS) -o $(EXIBIDOR) cliente_exibicao.cpp

# Limpa os executáveis criados
clean:
	rm -f $(SERVIDOR) $(CLIENTE) $(EXIBIDOR)

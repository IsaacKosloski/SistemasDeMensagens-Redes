#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTES 15  // Limite máximo de clientes
#define BUFFER_SIZE 256

// Estrutura da mensagem
struct Mensagem {
    int tipo;
    int remetente;
    int destinatario;
    int tamanho_texto;
    char nome[20];
    char texto[140];
};

// Array de clientes conectados
struct sockaddr_in clientes[MAX_CLIENTES];
int num_clientes = 0;  // Número atual de clientes conectados

// Função que envia uma mensagem para todos os clientes
void enviarMensagemParaTodos(int server_socket, struct Mensagem &msg) {
    for (int i = 0; i < num_clientes; i++) {
        sendto(server_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&clientes[i], sizeof(clientes[i]));
    }
}

// Função para adicionar um cliente ao array
void adicionarCliente(struct sockaddr_in novo_cliente) {
    if (num_clientes < MAX_CLIENTES) {
        clientes[num_clientes] = novo_cliente;
        num_clientes++;
    } else {
        std::cerr << "Número máximo de clientes atingido." << std::endl;
    }
}

// Função para remover um cliente do array
void removerCliente(struct sockaddr_in cliente) {
    for (int i = 0; i < num_clientes; i++) {
        if (memcmp(&clientes[i], &cliente, sizeof(cliente)) == 0) {
            for (int j = i; j < num_clientes - 1; j++) {
                clientes[j] = clientes[j + 1];
            }
            num_clientes--;
            break;
        }
    }
}

// Função principal do servidor
void servidor() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct Mensagem msg;

    // Criar socket UDP
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Erro ao criar socket!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular o socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erro ao fazer bind!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Servidor iniciado e aguardando mensagens..." << std::endl;

    // Loop principal para receber mensagens dos clientes
    while (true) {
        // Receber mensagem
        if (recvfrom(server_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&client_addr, &addr_len) < 0) {
            std::cerr << "Erro ao receber mensagem!" << std::endl;
            continue;
        }

        // Adicionar novo cliente à lista se for uma mensagem "OI"
        if (msg.tipo == 0) {  // Tipo OI
            std::cout << "Cliente " << msg.remetente << " conectado." << std::endl;
            adicionarCliente(client_addr);
            sendto(server_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&client_addr, addr_len);
        }

            // Mensagem de saída
        else if (msg.tipo == 1) {  // Tipo TCHAU
            std::cout << "Cliente " << msg.remetente << " desconectado." << std::endl;
            removerCliente(client_addr);
        }

            // Repassar mensagem para os clientes
        else if (msg.tipo == 2) {  // Tipo MSG
            std::cout << "Mensagem recebida de " << msg.remetente << ": " << msg.texto << std::endl;
            if (msg.destinatario == 0) {
                enviarMensagemParaTodos(server_socket, msg);
            }
        }
    }

    close(server_socket);
}

int main() {
    std::thread t(servidor);
    t.join();
    return 0;
}

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
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

// Função que recebe mensagens do servidor
void receberMensagens(int client_socket) {
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    struct Mensagem msg;

    while (true) {
        if (recvfrom(client_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&server_addr, &addr_len) > 0) {
            std::cout << "Mensagem recebida de " << msg.remetente << ": " << msg.texto << std::endl;
        }
    }
}

// Função principal do cliente
void cliente(int id, const std::string& nome) {
    int client_socket;
    struct sockaddr_in server_addr;
    struct Mensagem msg;

    // Criar socket UDP
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Erro ao criar socket!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Identificação do cliente
    msg.tipo = 0;
    msg.remetente = id;
    strncpy(msg.nome, nome.c_str(), sizeof(msg.nome));

    // Enviar mensagem de identificação (OI)
    sendto(client_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Iniciar thread para receber mensagens
    std::thread t(receberMensagens, client_socket);
    t.detach();

    // Loop para enviar mensagens
    while (true) {
        std::cout << "Digite a mensagem: ";
        std::string texto;
        std::getline(std::cin, texto);

        msg.tipo = 2;
        msg.destinatario = 0;  // Enviar para todos
        strncpy(msg.texto, texto.c_str(), sizeof(msg.texto));

        sendto(client_socket, &msg, sizeof(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    close(client_socket);
}

int main() {
    int id;
    std::string nome;

    std::cout << "Digite o seu identificador (número): ";
    std::cin >> id;
    std::cin.ignore();

    std::cout << "Digite o seu nome: ";
    std::getline(std::cin, nome);

    cliente(id, nome);
    return 0;
}

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

struct Mensagem {
    int tipo;
    int remetente;
    int destinatario;
    int tamanho_texto;
    char nome[20];
    char texto[140];
};

void exibirMensagem(const Mensagem& msg) {
    std::cout << "Mensagem de " << msg.nome 
              << " (ID: " << msg.remetente << "): "
              << msg.texto << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <ID> <IP> <Porta>\n";
        return 1;
    }

    int id = std::stoi(argv[1]);  // Identificador do cliente exibidor
    const char* ip = argv[2];     // IP do servidor
    int porta = std::stoi(argv[3]);  // Porta do servidor

    // Criação do socket UDP
    int socket_cliente = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_cliente < 0) {
        std::cerr << "Erro ao criar socket!\n";
        return 1;
    }

    struct sockaddr_in servidor_addr{};
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(porta);
    inet_pton(AF_INET, ip, &servidor_addr.sin_addr);

    // Enviar mensagem "OI" para se registrar no servidor
    Mensagem msg = {0, id, 0, 0, "", ""};  // Tipo 0 = OI
    sendto(socket_cliente, &msg, sizeof(msg), 0, 
           (struct sockaddr*)&servidor_addr, sizeof(servidor_addr));

    std::cout << "Cliente exibidor conectado (ID: " << id << ").\n";

    // Loop para receber e exibir mensagens
    while (true) {
        Mensagem msg_recebida;
        socklen_t addr_len = sizeof(servidor_addr);

        if (recvfrom(socket_cliente, &msg_recebida, sizeof(msg_recebida), 0, 
                     (struct sockaddr*)&servidor_addr, &addr_len) < 0) {
            std::cerr << "Erro ao receber mensagem!\n";
            continue;
        }

        exibirMensagem(msg_recebida);
    }

    close(socket_cliente);
    return 0;
}

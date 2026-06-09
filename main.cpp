#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Erreur socket" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Erreur bind" << std::endl;
        close(serverSocket);
        return 1;
    }
    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Erreur listen" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Serveur ouvert sur le port " << port << std::endl;

    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
    
    if (clientSocket >= 0) {
        std::cout << "Quelqu'un s'est connecté ! Succès." << std::endl;
        
        std::string msg = "Bravo, la socket fonctionne !\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        
        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
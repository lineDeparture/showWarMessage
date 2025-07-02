// tcp_server_loop.cpp
#include "save_log.h"
#include "server.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <ctime>
#include <mutex>

#define PORT 8080
#define BUFFER_SIZE 1024

// std::mutex logMutex;

void runServer(const std::string& logDir)
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        return;
    }

    listen(server_fd, 3);
    std::cout << "서버 대기 중...\n";

    while (true) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        std::cout << "클라이언트 연결됨.\n";

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(client_fd, buffer, BUFFER_SIZE - 1);
            if (valread <= 0) {
                std::cout << "클라이언트 연결 종료\n";
                close(client_fd);
                break;
            }

            std::string received(buffer, valread);
            std::cout << "받은 데이터: " << received << std::endl;

            saveLog(logDir, "ClientLog", received);

            std::string response = "ack:" + received;
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }

    close(server_fd);
}
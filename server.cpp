// server.cpp
#include "save_log.h"
#include "server.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <ctime>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void runServer(const std::string& logDir)
{
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET, client_fd = INVALID_SOCKET;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    int addrlen = sizeof(address);

    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup 실패\n";
        return;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패: " << WSAGetLastError() << "\n";
        WSACleanup();
        return;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt 오류\n";
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "bind 실패: " << WSAGetLastError() << "\n";
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "listen 실패\n";
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    std::cout << "서버 대기 중...\n";

    while (true) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "accept 오류: " << WSAGetLastError() << "\n";
            continue;
        }
        std::cout << "클라이언트 연결됨.\n";

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (valread <= 0) {
                std::cout << "클라이언트 연결 종료\n";
                closesocket(client_fd);
                break;
            }

            std::string received(buffer, valread);
            std::cout << "받은 데이터: " << received << std::endl;

            saveLog(logDir, "ClientLog", received);

            std::string response = "ack:" + received;
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }

    closesocket(server_fd);
    WSACleanup();
}

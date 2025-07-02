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
#include <sys/stat.h>
#include <sys/types.h>

#define TEXT_LOG_PORT 8080
#define VIDEO_LOG_PORT 8081
#define BUFFER_SIZE 1024


void runTextLogServer(const std::string& logDir)
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
    address.sin_port = htons(TEXT_LOG_PORT);

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


std::string current_timestamp() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localtime(&now));
    return std::string(buf);
}

void runVideoLogServer() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(VIDEO_LOG_PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) { perror("bind"); return; }
    listen(server_fd, 3);
    std::cout << "[VideoServer] Waiting on port " << VIDEO_LOG_PORT << "...\n";

    while (true) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        std::cout << "[VideoServer] Client connected.\n";

        // 헤더: 파일명:파일크기
        char header[256] = {0};
        int header_read = read(client_fd, header, 256);
        if (header_read <= 0) {
            std::cerr << "[VideoServer] Failed to read header.\n";
            close(client_fd);
            continue;
        }

        std::string header_str(header);
        size_t delim_pos = header_str.find(':');
        if (delim_pos == std::string::npos) {
            std::cerr << "[VideoServer] Invalid header format.\n";
            close(client_fd);
            continue;
        }

        std::string filename = header_str.substr(0, delim_pos);
        long filesize = std::stol(header_str.substr(delim_pos + 1));

        mkdir("./received_videos", 0777);
        std::string new_filename = "./received_videos/" + current_timestamp() + "_" + filename;

        std::ofstream outfile(new_filename, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "[VideoServer] Failed to open file.\n";
            close(client_fd);
            continue;
        }

        long received = 0;
        while (received < filesize) {
            int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) break;

            outfile.write(buffer, bytes_read);
            received += bytes_read;
        }

        outfile.close();
        std::cout << "[VideoServer] File saved: " << new_filename << "\n";

        close(client_fd);
        std::cout << "[VideoServer] Client disconnected.\n";
    }

    close(server_fd);
}
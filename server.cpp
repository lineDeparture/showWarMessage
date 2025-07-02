// tcp_server_loop.cpp
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




std::mutex logMutex;

std::string getCurrentTime()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday,
             ltm->tm_hour,
             ltm->tm_min,
             ltm->tm_sec);
    return std::string(buf);
}

void saveLog(const std::string& filePath, const std::string& section, const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    // 기존 섹션 존재 확인
    std::ifstream inFile(filePath);
    bool sectionExists = false;
    if (inFile.is_open()) {
        std::string line;
        while (getline(inFile, line)) {
            if (line == "[" + section + "]") {
                sectionExists = true;
                break;
            }
        }
        inFile.close();
    }

    std::ofstream outFile(filePath, std::ios::app);

    // 섹션이 없으면 새로 작성
    if (!sectionExists) {
        outFile << "[" << section << "]\n";
    }

    std::string timestamp = getCurrentTime();
    outFile << timestamp << " = " << message << "\n";
    outFile.close();

    std::cout << "로그 저장 완료: " << timestamp << " - " << message << std::endl;
}



int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }

    listen(server_fd, 3);
    std::cout << "서버 대기 중...\n";

    client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }

    std::cout << "클라이언트 연결됨.\n";

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (valread <= 0) {
            printf("client_fd: %d, valread: %d\n", client_fd, valread);
            std::cout << "클라이언트 연결 종료\n";
            break;
        }

        std::string received(buffer, valread);
        std::cout << "받은 데이터: " << received << std::endl;

        // 응답 전송
        std::string response = "ack:" + received;
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}

// tcp_server_loop.cpp
#include "save_log.h"
#include "server.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <sys/stat.h>
#include <cstring>
#include <ctime>

int main() {


    std::string logDir = "logs"; // 기본 폴더
    std::thread logThread([&logDir]() { runTextLogServer(logDir); });
    std::thread videoThread(runVideoLogServer);

    logThread.join();
    videoThread.join();

    std::cout << "서버 종료.\n";
    return 0;
}

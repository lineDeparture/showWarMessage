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

int main() {

    std::string logDir = "logs"; // 기본 폴더

    runServer(logDir);

    return 0;
}

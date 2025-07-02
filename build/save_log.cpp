#include "save_log.h"
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

std::string getCurrentDate()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday);
    return std::string(buf);
}
void createLogDir(const std::string& dirPath)
{
    struct stat st;
    if (stat(dirPath.c_str(), &st) != 0) {
        // 디렉토리가 없으면 생성
        mkdir(dirPath.c_str(), 0755);
    }
}

void saveLog(const std::string& dirPath, const std::string& section, const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    createLogDir(dirPath);

    std::string date = getCurrentDate();
    std::string filePath = dirPath + "/logs_" + date + ".ini";

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
    if (!sectionExists) {
        outFile << "[" << section << "]\n";
    }

    std::string timestamp = getCurrentTime();
    outFile << timestamp << " = " << message << "\n";
    outFile.close();

    std::cout << "로그 저장 완료: " << timestamp << " - " << message << std::endl;
}
#include "save_log.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <mutex>

#ifdef _WIN32
    #include <direct.h>     // _mkdir
    #define mkdir(dir, mode) _mkdir(dir)   // 리눅스와 함수명 통일
    #define PATH_SEPARATOR '\\'
#else
    #include <sys/stat.h>
    #define PATH_SEPARATOR '/'
#endif

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
    // 폴더가 없으면 생성
    std::ifstream dirTest(dirPath);
    if (!dirTest.good()) {
        mkdir(dirPath.c_str(), 0755);
    }
}

void saveLog(const std::string& dirPath, const std::string& section, const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    createLogDir(dirPath);

    std::string date = getCurrentDate();
    std::string filePath = dirPath + PATH_SEPARATOR + "logs_" + date + ".ini";

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
cmake_minimum_required(VERSION 3.10)
project(MyServerApp)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 헤더 파일 포함 디렉토리
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# 소스 파일 목록
set(SOURCES
    main.cpp
    server.cpp
    save_log.cpp
)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SOURCES})

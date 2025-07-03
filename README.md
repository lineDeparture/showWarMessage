# Lane Detection with YOLO Object Detection

이 프로젝트는 차선 검출과 YOLO 객체 검출을 결합한 자동차 안전 시스템에서 얻은 정보를 전달받고, 로그에 저장하는 서버 역할을 합니다. 


## 기능

- **서버 생성**: Lane Detection with YOLO object Detection 프로젝트와 연결할 수 있는 TCP Socket server 생성
- **로그 저장**: 전달받은 데이터를 저장하는 로그 파일 생성 및 저장


## 서버 실행방법 
./tcp_server_log_split server


## 파일 구조

```
ShowWarMessage/
├── main.cpp                # 프로그램 시작지점 
├── server.cpp              # TCP Socket server 생성, 데이터 송.수신, 로그저장 요청
├── server.h                # server.cpp 파일 추상화
├── save_log.cpp            #  로그 저장
├── save_log.h              #  save_log.cpp 파일 추상화 
├── logs                    # 로그파일 저장하는 폴더 
  --- .....                 # 로그파일 리스트 



```



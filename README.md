# RemoteExplorer
Remote Explorer C++

## 목적
- 프로그램의 **설계**부터 **구현**까지 전 과정을 작업해보는것

## 종류
### 1. 윈도우 기본
#### 기능
- 파일 업로드 / 다운로드

### 2. [FileZilla](https://filezilla-project.org)
#### 기능
- 원격 접속
- 파일 추가 / 삭제
- 파일 업로드 / 다운로드
- 메시지 로그

### 3. [포인트락](http://www.pointlock.co.kr)
#### 기능
- 원격 접속
- 파일 추가 / 삭제
- 파일 업로드 / 다운로드
- 파일 암호화 / 복호화
- 폴더, 저장장치, 네트워크 잠금

### 4. [RemoteView](https://www.rview.com/ko/login)
#### 기능
- 원격 접속
- 접속시 2단계 인증
- 파일 업로드 / 다운로드
- Drag & Drop, Copy & Paste 방식으로 원격지 PC에 전송
- 원격 프린터
- 파일 전송 이력 확인 가능
- 권한을 통한 접근 설정 가능
- 자주 사용하는 PC나 사용자 즐겨찾기

### 5. [RemoteExplorer](https://remote-explorer.kr.uptodown.com/windows)
#### 기능
- 원격 접속
- 파일 업로드 / 다운로드
- 파일 추가 / 삭제
- 파일 비교
- 파일 검색

### To do list Remote Explorer By tdm1223
- [ ] 원격 접속 (ip + port)
- [ ] 정렬 기능(각 항목별)
- [ ] 파일 추가
- [ ] 파일 삭제
- [ ] 파일 업로드
- [ ] 파일 다운로드

## 1차 설계
### Client
- Recv thread
- Recv queue
- Send thread
- Send queue
- 동시전송은 하지 않는다.

### Server
- Event Select 모델을 활용한다.
- 서버를 설계하는데 중요하게 생각할것
    1. 다수의 클라이언트가 동시 접속시 흐름 관리
    2. 스레드 사용
    
### 클래스 다이어그램 (주요 함수 위주로 작성 해보기)
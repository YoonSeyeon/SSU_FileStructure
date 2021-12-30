# 디스크 I/O 비용 측정

### 학생 레코드 파일을 생성하는 프로그램을 구현(create_file.c)
* 고정길이레코드 방식으로 250 Byte 레코드를 레코드 파일에 저장
* 레코드 파일 맨 앞에 4 Byte 헤더 레코드를 두며, 레코드 파일에 저장되어 있는 레코드의 수를 저장
* 헤더 레코드 다음 Byte 부터 첫 번째 레코드를 저장
* 레코드에 저장되는 데이터는 임의의 문자를 저장   

입력 : ./<실행파일명> <레코드 수> <레코드파일명>    
구현내용 : 주어진 레코드 수만큼의 레코드를 주어진 레코드 파일에 저장
- - -
### 레코드 파일에 저장된 레코드를 순차적으로 읽는 프로그램을 구현(read_seq.c)

실행방법 : ./<실행파일명> <레코드파일명>   
구현내용 : 파일에 저장되어 있는 레코드를 순차적으로 읽고 실행 시간 출력
- - -
### 레코드 파일에 저장된 레코드를 랜덤으로 읽는 프로그램을 구현(read_random.c)

실행방법 : ./<실행파일명> <레코드파일명>    
구현내용 : 파일에 저장되어 있는 레코드를 랜덤으로 읽고 실행 시간 출력
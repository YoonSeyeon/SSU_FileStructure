#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define OPEN_FLAG O_RDWR|O_CREAT|O_TRUNC
#define OPEN_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH

int main(int argc, char *argv[])
{
	char *o_file = argv[1];		// 원본파일
	char *c_file = argv[2];		// 복사본파일
	int o_file_fd, c_file_fd;	// 원본파일과 복사본파일의 파일 디스크립터

	int length;
	char data[10];
	
	if(argc != 3){
		fprintf(stderr, "[ERROR] %s <원본파일명> <복사본파일명> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((o_file_fd = open(o_file, O_RDONLY)) == -1){				// 원본파일을 읽기 모드로 열기
		fprintf(stderr, "[ERROR] <%s> open error\n", o_file);	// 원본파일 open 실패 시 에러
		exit(1);
	}

	if((c_file_fd = open(c_file, OPEN_FLAG, OPEN_MODE)) == -1){	// 복사본파일을 읽기/쓰기 모드로 생성
		fprintf(stderr, "[ERROR] <%s> open error\n", c_file);	// 복사본파일 open 실패 시 에러
		exit(1);
	}

	while((length = read(o_file_fd, data, 10)) > 0){			// 원본파일로 부터 10바이트씩 읽어 data에 저장
		write(c_file_fd, data, length);							// 복사본 파일에 data의 값 10 씩 저장
	}

	return 0;
}


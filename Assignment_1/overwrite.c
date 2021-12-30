#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *fname = argv[1];			// 파일명
	char *data = argv[3];			// 데이터
	int fd;
	off_t offset = atoi(argv[2]);	// 오프셋
	
	if(argc != 4){
		fprintf(stderr, "[ERROR] %s <파일명> <오프셋> <데이터> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((fd = open(fname, O_RDWR)) == -1){		// overwrite할 파일 열기
		fprintf(stderr, "[ERROR] <%s> open error!\n", fname);
		exit(1);
	}
	
	if((lseek(fd, offset, SEEK_SET)) == -1){	// 오프셋 위치 변경
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	if(write(fd, data, strlen(data)) < 0){		// 오프셋 위치부터 overwrite 하기
		fprintf(stderr, "[ERROR] write error!\n");
		exit(1);
	}
	
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 100000

int main(int argc, char *argv[])
{
	char *fname = argv[1];			// 파일명
	char *data = argv[3];			// 데이터
	int fd;
	int length;						// temp에 저장할 내용의 길이

	char temp[BUFFER_SIZE];			// overwrite되지 않게 임시적으로 오프셋+1 에서 파일 끝까지의 데이터를 저장할 충분히 큰 공간
	off_t offset = atoi(argv[2]);	// 오프셋
	
	if(argc != 4){
		fprintf(stderr, "[ERROR] %s <파일명> <오프셋> <데이터> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((fd = open(fname, O_RDWR)) == -1){			// 끼워넣기 할 파일 오픈 
		fprintf(stderr, "[ERROR] <%s> open error!\n", fname);
		exit(1);
	}
	
	if(lseek(fd, offset+1, SEEK_SET) == -1){		// 오프셋+1의 위치로 오프셋 변경
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	length = read(fd, temp, BUFFER_SIZE);			// 오프셋+1 부터 파일 끝까지의 파일 내용을 temp에 저장
	
	if(lseek(fd, offset+1, SEEK_SET) == -1){		// 다시 파일 처음부터의 오프셋+1의 위치로 오프셋 변경
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	if(write(fd, data, strlen(data)) < 0){			// <데이터>의 내용을 오프셋과 오프셋+1 사이에 끼워 넣는다.
		fprintf(stderr, "[ERROR] write error!\n");
		exit(1);
	}
	
	if(write(fd, temp, length) < 0){				// 데이터 뒤에 원래 오프셋+1 부터의 파일 내용을 붙여 넣는다.
		fprintf(stderr, "[ERROR] write error!\n");
		exit(1);
	}
	
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *fname = argv[1];			// 파일명
	char buf[100000];				// 파일에서 읽은 데이터를 저장
	int fd;
	
	off_t offset = atoi(argv[2]);	// 오프셋
	size_t nbytes = atoi(argv[3]);	// 읽기 바이트 수
	
	if(argc != 4){
		fprintf(stderr, "[ERROR] %s <파일명> <오프셋> <읽기 바이트 수> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((fd = open(fname, O_RDONLY)) == -1){		// 읽을 파일 열기
		fprintf(stderr, "[ERROR] <%s> open error!\n", fname);
		exit(1);
	}
	
	if((lseek(fd, offset, SEEK_SET)) == -1){	// 오프셋 위치 변경
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	read(fd, buf, nbytes);	// 오프셋에서부터 nbytes 만큼의 데이터를 buf에 저장
	printf("%s", buf);		// 읽은 데이터를 출력

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *fname = argv[1];			// 파일명
	char temp[100000];				// (오프셋 + 삭제할 바이트 수)의 오프셋부터 파일 끝까지의 데이터 임시 저장
	int fd;
	int length;						// temp에 저장된 데이터의 길이

	off_t offset = atoi(argv[2]);	// 오프셋
	size_t nbytes = atoi(argv[3]);	// 삭제 바이트 수
	
	if(argc != 4){
		fprintf(stderr, "[ERROR] %s <파일명> <오프셋> <삭제 바이트 수> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((fd = open(fname, O_RDWR)) == -1){ 				// 삭제하기 할 파일 열기(쓰기 기능도 필요)
		fprintf(stderr, "[ERROR] <%s> open error!\n", fname);
		exit(1);
	}
	

	if(lseek(fd, offset + nbytes, SEEK_SET) == -1){	// 오프셋 위치 변경
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	length = read(fd, temp, sizeof(temp));		// 삭제할 데이터 뒤의 내용 임시 저장
	
	if(lseek(fd, offset, SEEK_SET) == -1){		// 삭제할 데이터가 있는 오프셋으로 이동
		fprintf(stderr, "[ERROR] lseek error!\n");
		exit(1);
	}

	write(fd, temp, length);					// 임시 저장했던 데이터를 덮어씀
	ftruncate(fd, offset + length);				// 삭제 데이터 공간 만큼 파일에서 삭제

	return 0;
}


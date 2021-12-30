#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1000

int main(int argc, char *argv[])
{
	char *mergefile = argv[1];		// 병합하여 생성할 파일 <파일명 1>
	char *file1 = argv[2];			// 병합할 파일 <파일명 2>
	char *file2 = argv[3];			// 병합할 파일 <파일명 3>
	int m_fd, fd1, fd2;
	int length;
	char buf[SIZE];
		
	if(argc != 4){
		fprintf(stderr, "[ERROR] %s <파일명1> <파일명2> <파일명3> 형식으로 실행해주세요.\n", argv[0]);
		exit(1);
	}

	if((m_fd = open(mergefile, O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1){	// 병합하여 생성할 파일 생성 및 열기
		fprintf(stderr, "[ERROR] <%s> creat error!\n", mergefile);
		exit(1);
	}
	
	if((fd1 = open(file1, O_RDONLY)) == -1){			// 병합할 첫 번째 파일 열기
		fprintf(stderr, "[ERROR] <%s> open error!\n", file1);
		exit(1);
	}
	else{
		while((length = read(fd1, buf, SIZE)) > 0)		// 성공적으로 open하였으면, 파일에서 데이터 읽은 후, 생성한 파일에 쓰기
			write(m_fd, buf, length);
	}

	if((fd2 = open(file2, O_RDONLY)) == -1){			// 병합할 두 번째 파일 열기
		fprintf(stderr, "[ERROR] <%s> open error!\n", file2);
		exit(1);
	}
	else{
		while((length = read(fd2, buf, SIZE)) > 0)		// 성공적으로 open하였으면, 파일에서 데이터 읽은 후, 생성한 파일 맨 뒤에 붙여쓰기
			write(m_fd, buf, length);
	}

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define RECORD_SIZE 250

int main(int argc, char **argv)
{
	struct timeval start, end;
	FILE *fp;
	char buf[RECORD_SIZE];
	int length;
	int records_num;	// 레코드 수
	int timecost;		// 레코드 순차적으로 읽는 데 드는 시간적 비용

	if (argc != 2) {	// 실행 명령어, 레코드 파일로 입력하지 않으면 에러
		printf("usage: <%s> <record_file>\n", argv[0]);
		return 0;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL) {	// 파일 열기에 실패하면 에러 출력
		printf("open error for %s\n", argv[1]);
		return 0;
	}

	fread(&records_num, sizeof(int), 1, fp);	// 헤더 레코드에서 레코드 수 읽기

	gettimeofday(&start, NULL);
	for (int i = 0; i < records_num; i++) {		// 레코드 읽으면서 시간적 비용을 측정
		length = fread(buf, sizeof(char), RECORD_SIZE, fp);
		if (length != RECORD_SIZE) {
			printf("read error!\n");
			return 0;
		}
	}
	gettimeofday(&end, NULL);

	timecost = (int)(((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec));
	printf("#records: %d elapsed_time: %d us\n", records_num, timecost);
	
	fclose(fp);

	return 0;
}

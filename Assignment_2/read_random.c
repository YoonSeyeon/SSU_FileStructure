#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define SUFFLE_NUM	100000
#define RECORD_SIZE 250

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{
	FILE *fp;
	int *read_order_list;
	int records_num; 		// 레코드 파일에 저장되어 있는 전체 레코드의 수
	int length;
	long position;
	char buf[RECORD_SIZE];

	struct timeval start, end;
	int timecost;

	if (argc != 2) {		// 실행 명령어, 레코드 파일로 입력하지 않으면 에러
		printf("usage: <%s> <record_file>\n", argv[0]);
		return 0;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL) {	// 파일 열기에 실패하면 에러 출력
		printf("open error for %s\n", argv[1]);
		return 0;
	}
	
	fread(&records_num, sizeof(int), 1, fp);	// 헤더 레코드에서 레코드 수 읽기
	
	read_order_list = (int *)malloc(sizeof(int) * records_num);

	GenRecordSequence(read_order_list, records_num);

	gettimeofday(&start, NULL);
	for (int i = 0; i < records_num; i++) {		// 레코드 읽으면서 시간적 비용을 측정
		position = (long)read_order_list[i];
		fseek(fp, position, SEEK_SET);
		length = fread(buf, sizeof(char), RECORD_SIZE, fp);
		if (length != RECORD_SIZE) {
			printf("read error!\n");
			return 0;
		}
	}
	gettimeofday(&end, NULL);

	timecost = (int)(((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec));
	printf("#records: %d elapsed_time: %d us\n", records_num, timecost);

	free(read_order_list);		// 동적 할당한 리스트 해제
	fclose(fp);					// 파일 닫기

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}
	
	for (i = 0; i < SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

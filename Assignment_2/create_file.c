#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RECORD_SIZE 250

// record 내용
void inputRecordData(char *record) {
	memset(record, 0, RECORD_SIZE);
	for (int i = 0; i < RECORD_SIZE; i++) {
		if (i < 20) record[i] = 'a';
		else if (i < 40) record[i] = '~';
		else if (i < 60) record[i] = '?';
		else if (i < 80) record[i] = '!';
		else if (i < 100) record[i] = '@';
		else if (i < 120) record[i] = '#';
		else if (i < 140) record[i] = '$';
		else if (i < 160) record[i] = '%';
		else if (i < 180) record[i] = '(';
		else if (i < 200) record[i] = '*';
		else if (i < 250) record[i] = '/';
	}
}

int main(int argc, char **argv)
{
	FILE *fp;
	char record[RECORD_SIZE];
	int records_num;

	if (argc < 3) {								// 실행 명령어, 레코드 수, 레코드 파일명 순으로 입력하지 않을 시 에러
		printf("usage: %s <records_num> <record_file>\n", argv[0]);
		return 0;
	}

	if ((fp = fopen(argv[2], "w+b")) == NULL) {	// 파일 열기 실패 시, 에러 출력
		printf("open error for %s\n", argv[2]);
		return 0;
	}
	
	inputRecordData(record);		// 레코드에 값 입력
	records_num = atoi(argv[1]);	// 인자로 받은 레코드 수를 변수에 저장

	fwrite(&records_num, sizeof(records_num), 1, fp);		// 레코드 수를 저장한 4바이트 짜리 헤더 레코드를 레코드 파일 맨 앞에 저장
	for(int i = 0; i < records_num; i++){
		fwrite(record, sizeof(char), sizeof(record), fp);	// 레코드 수만큼의 레코드를 레코드 파일에 저장
	}

	fclose(fp);

	return 0;
}

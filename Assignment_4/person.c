#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "person.h"

char recordbuf[MAX_RECORD_SIZE];
char pagebuf[PAGE_SIZE];
char header_area[HEADER_AREA_SIZE];
char data_area[DATA_AREA_SIZE];
int header_record[4];
int metadata;
int max_records = (HEADER_AREA_SIZE - 4) / 8;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, 16 + (pagenum * PAGE_SIZE), SEEK_SET);
	if (fread(pagebuf, PAGE_SIZE, 1, fp) != 1) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}
}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, 16 + (pagenum * PAGE_SIZE), SEEK_SET);
	if (fwrite(pagebuf, PAGE_SIZE, 1, fp) != 1) {
		fprintf(stderr, "fwrite error\n");
		exit(1);
	}
}

void pack(char *recordbuf, const Person *p)
{
	strcat(recordbuf, p->id);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->name);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->age);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->addr);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->phone);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->email);
	strcat(recordbuf, "#");
}

void add(FILE *fp, const Person *p)
{
	int num_of_records;
	int offset;
	int tempoffset;
	int currlength;
	int templength;
	int next_page;
	int next_record;
	int prev_page;
	int prev_record;
	int curr_page;
	int curr_record;
	int empty_size;
	char delete_mark = '*';
	char tempbuf[PAGE_SIZE];
	bool append = false;

	memset(header_area, 0, HEADER_AREA_SIZE);
	memset(data_area, 0, DATA_AREA_SIZE);

	pack(recordbuf, p);
	currlength = strlen(recordbuf);
	if (fread(header_record, sizeof(int), 4, fp) != 4) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}

	if (header_record[0] == 0) {	// first write 
		num_of_records = 1;
		offset = 0;
		header_record[0] = 1;
		header_record[1] = 1;
		rewind(fp);
		fwrite(header_record, sizeof(int), 4, fp);

		memcpy(header_area, &num_of_records, 4);
		memcpy(header_area + 4, &offset, 4);
		memcpy(header_area + 8, &currlength, 4);
		memcpy(data_area + offset, recordbuf, currlength);
		memcpy(pagebuf, header_area, HEADER_AREA_SIZE);
		memcpy(pagebuf + HEADER_AREA_SIZE, data_area, DATA_AREA_SIZE);
		writePage(fp, pagebuf, 0);
	}
	else {
		if (header_record[2] >= 0) {	
			curr_page = header_record[2];
			curr_record = header_record[3];

			while (1) {
				if (curr_page < 0) {
					append = true;
					break;
				}

				readPage(fp, pagebuf, curr_page);
				memcpy(header_area, pagebuf, HEADER_AREA_SIZE);
				memcpy(data_area, pagebuf + HEADER_AREA_SIZE, DATA_AREA_SIZE);
				memcpy(&offset, (header_area + 4) + (curr_record * 8), sizeof(int));
				memcpy(&templength, (header_area + 8) + (curr_record * 8), sizeof(int));
				memcpy(&next_page, data_area + offset + 1, sizeof(int));
				memcpy(&next_record, data_area + offset + 5, sizeof(int));

				if (templength < currlength) {
					prev_page = curr_page;
					prev_record = curr_record;
					curr_page = next_page;
					curr_record = next_record;
				}
				else {	// right size
					if ((curr_page == header_record[2]) && (curr_record == header_record[3])) { 
						header_record[2] = next_page;
						header_record[3] = next_record;
						rewind(fp);
						fwrite(header_record, sizeof(int), 4, fp);
						memcpy(data_area + offset, recordbuf, currlength);
					}
					else {
						readPage(fp, tempbuf, prev_page);
						memcpy(&tempoffset, tempbuf + 4 + (prev_record * 8), sizeof(int));
						memcpy(tempbuf + HEADER_AREA_SIZE + tempoffset + 1, &next_page, sizeof(int));
						memcpy(tempbuf + HEADER_AREA_SIZE + tempoffset + 5, &next_record, sizeof(int));
						writePage(fp, tempbuf, prev_page);
						memcpy(data_area + offset, recordbuf, currlength);
					}
					memcpy(pagebuf, header_area, HEADER_AREA_SIZE);
					memcpy(pagebuf + HEADER_AREA_SIZE, data_area, DATA_AREA_SIZE);
					writePage(fp, pagebuf, curr_page);
					break;
				}
			}
		}
		else {
			append = true;
		}

		if (append) {
			curr_page = header_record[0] - 1;
			readPage(fp, pagebuf, curr_page);
			memcpy(header_area, pagebuf, HEADER_AREA_SIZE);
			memcpy(data_area, pagebuf + HEADER_AREA_SIZE, DATA_AREA_SIZE);
			memcpy(&num_of_records, header_area, sizeof(int));
			empty_size = DATA_AREA_SIZE - strlen(data_area);
			
			if ((num_of_records < max_records) && (currlength <= empty_size)) {
				header_record[1]++;
				
				memcpy(&tempoffset, (header_area + 4) + (num_of_records - 1) * 8, sizeof(int));
				memcpy(&templength, (header_area + 8) + (num_of_records - 1) * 8, sizeof(int));
				offset = tempoffset + templength;

				memcpy((header_area + 4) + (num_of_records * 8), &offset, sizeof(int));
				memcpy((header_area + 8) + (num_of_records * 8), &currlength, sizeof(int));
				num_of_records++;
				memcpy(header_area, &num_of_records, sizeof(int));
				memcpy(data_area + offset, recordbuf, currlength);
				memcpy(pagebuf, header_area, HEADER_AREA_SIZE);
				memcpy(pagebuf + HEADER_AREA_SIZE, data_area, DATA_AREA_SIZE);
				writePage(fp, pagebuf, curr_page);
			}
			else { 
				memset(header_area, 0, HEADER_AREA_SIZE);
				memset(data_area, 0, DATA_AREA_SIZE);

				num_of_records = 1;
				offset = 0;
				header_record[0]++;
				header_record[1]++;

				memcpy(header_area, &num_of_records, 4);
				memcpy(header_area + 4, &offset, 4);
				memcpy(header_area + 8, &currlength, 4);
				memcpy(data_area + offset, recordbuf, currlength);
				memcpy(pagebuf, header_area, HEADER_AREA_SIZE);
				memcpy(pagebuf + HEADER_AREA_SIZE, data_area, DATA_AREA_SIZE);
				writePage(fp, pagebuf, curr_page + 1);
			}
			
			rewind(fp);
			fwrite(header_record, sizeof(int), 4, fp);
		}
	}	
}

void delete(FILE *fp, const char *id)
{
	Person p;
	int num_of_records;
	int offset;
	int length;
	char delete_mark = '*';
	char *pch;
	bool finding = false;

	if (fread(header_record, sizeof(int), 4, fp) != 4) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}

	for (int i = 0; i < header_record[0]; i++) {
		readPage(fp, pagebuf, i);
		memcpy(header_area, pagebuf, HEADER_AREA_SIZE);
		memcpy(data_area, pagebuf + HEADER_AREA_SIZE, DATA_AREA_SIZE);
		memcpy(&num_of_records, header_area, sizeof(int));

		for (int j = 0; j < num_of_records; j++) {
			memcpy(&offset, (header_area + 4) + (j * 8), sizeof(int));
			memcpy(&length, (header_area + 8) + (j * 8), sizeof(int));
			memcpy(&recordbuf, data_area + offset, length);
			pch = strtok(recordbuf, "#");

			if (strcmp(pch, id) == 0) {
				memcpy(data_area + offset, &delete_mark, 1);
				memcpy(data_area + offset + 1, &header_record[2], 4);
				memcpy(data_area + offset + 5, &header_record[3], 4);
				memcpy(&header_record[2], &i, 4);
				memcpy(&header_record[3], &j, 4);
				finding = true;
				break;
			}
		}

		if (finding) {
			rewind(fp);
			fwrite(header_record, sizeof(int), 4, fp);
			memcpy(pagebuf, header_area, HEADER_AREA_SIZE);
			memcpy(pagebuf + HEADER_AREA_SIZE, data_area, DATA_AREA_SIZE);
			writePage(fp, pagebuf, i);
			break;			
		}
	}

	if (!finding)
		fprintf(stderr, "This id could not be found in the record file.\n");
}

int main(int argc, char *argv[])
{
	FILE *fp;
	Person p;

	if (argc < 2)
		exit(0);

	if (strcmp(argv[1], "a") == 0) {
		if (argc < 9) {
			fprintf(stderr, "Usage : %s a <record file name> <field values list>\n", argv[0]);
			exit(1);
		}

		if ((fp = fopen(argv[2], "r+")) == NULL) {
			if ((fp = fopen(argv[2], "w+")) == NULL) {
				fprintf(stderr, "file open error for %s\n", argv[2]);
				exit(1);
			}
			metadata = 0;
			memcpy(&header_record[0], &metadata, 4);
			memcpy(&header_record[1], &metadata, 4);
			metadata = -1;
			memcpy(&header_record[2], &metadata, 4);
			memcpy(&header_record[3], &metadata, 4);
			fwrite(header_record, sizeof(int), 4, fp);
			rewind(fp);
		}

		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);
		add(fp, &p);
	}
	else if (strcmp(argv[1], "d") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage : %s d <record file name> <field value>\n", argv[0]);
			exit(1);
		}

		if ((fp = fopen(argv[2], "r+")) == NULL) {
			fprintf(stderr, "no file to delete records\n");
			exit(1);
		}
		delete(fp, argv[3]);
	}
	else {
		fprintf(stderr, "Program error! Please input correct sentence!\n");
		exit(1);
	}

	fclose(fp);

	return 0;
}

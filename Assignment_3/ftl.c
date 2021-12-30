#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"
#include <stdbool.h>

int dd_read(int, char *);
int dd_write(int, char *);
int dd_erase(int);
int mapping_table[BLOCKS_PER_DEVICE];
bool pbn_check[BLOCKS_PER_DEVICE];
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
void ftl_open()
{
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    // address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	int lbn;

	for(int i = 0; i < DATABLKS_PER_DEVICE; i++){
		mapping_table[i] = -1;
	}
	for(int i = 0; i < BLOCKS_PER_DEVICE; i++){
		if(dd_read(i*PAGES_PER_BLOCK, pagebuf) > 0){
			memcpy(&lbn, &pagebuf[SECTOR_SIZE], 4);
			if(lbn >= 0){
				mapping_table[lbn] = i;
				pbn_check[i] = true;
			}
		}
	}
	for(int i = 0; i < BLOCKS_PER_DEVICE; i++){
		if(pbn_check[i] == false){
			mapping_table[DATABLKS_PER_DEVICE] = i;
			pbn_check[i] = true;
			break;
		}
	}
	free(pagebuf);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
	int lbn, pbn, ppn, offset;
	char *pagebuf = (char *)malloc(PAGE_SIZE);

	lbn = lsn / PAGES_PER_BLOCK;
	offset = lsn % PAGES_PER_BLOCK;
	pbn = mapping_table[lbn];
	ppn = pbn * PAGES_PER_BLOCK + offset;
	if(dd_read(ppn, pagebuf) == 1){
		memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
	}
	free(pagebuf);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
	int lbn, pbn, ppn, offset, check;
	char *pagebuf = (char *)malloc(PAGE_SIZE);

	memset(pagebuf, 0xff, PAGE_SIZE);
	lbn = lsn / PAGES_PER_BLOCK;
	offset = lsn % PAGES_PER_BLOCK;
	if(mapping_table[lbn] >= 0){ // lbn에 pbn 할당
		pbn = mapping_table[lbn];
		ppn = pbn * PAGES_PER_BLOCK + offset;
		if(dd_read(ppn, pagebuf) > 0){
			memcpy(&check, &pagebuf[SECTOR_SIZE+4], 4);
			if(check >= 0){	// 갱신(update)
				// 기존 블록의 각 페이지의 데이터를 프리 블록으로 복사
				for(int i = 0; i < PAGES_PER_BLOCK; i++){
					if(i == offset)
						continue;
					if(dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf) < 0){
						fprintf(stderr, "dd_read error\n");
						exit(1);
					}
					else{
						if(dd_write(mapping_table[DATABLKS_PER_DEVICE] * PAGES_PER_BLOCK + i, pagebuf) < 0){
							fprintf(stderr, "dd_write error\n");
							exit(1);
						}
					}
				}

				// 프리 블록에 sectorbuf 값 저장
				memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
				memcpy(&pagebuf[SECTOR_SIZE], &lbn, 4);
				memcpy(&pagebuf[SECTOR_SIZE + 4], &lsn, 4);
				if(dd_write(mapping_table[DATABLKS_PER_DEVICE] * PAGES_PER_BLOCK + offset, pagebuf) < 0){
					fprintf(stderr, "dd_write error\n");
					exit(1);
				}
				
				// 기존 블록 erase
				if(dd_erase(pbn) < 0){
					fprintf(stderr, "dd_erase error\n");
					exit(1);
				}
				
				// 프리 블록을 기존 블록으로 바꿈
				mapping_table[lbn] = mapping_table[DATABLKS_PER_DEVICE];
				mapping_table[DATABLKS_PER_DEVICE] = pbn;
			}
			else{
				memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
				memcpy(&pagebuf[SECTOR_SIZE], &lbn, 4);
				memcpy(&pagebuf[SECTOR_SIZE + 4], &lsn, 4);
				dd_write(ppn, pagebuf);
			}
		}
	}
	else{ // 블록 자체를 처음 방문
		for(int i = 0; i < BLOCKS_PER_DEVICE; i++){
			if(pbn_check[i] == false){
				mapping_table[lbn] = i;
				pbn_check[i] = true;
				break;
			}
		}
		pbn = mapping_table[lbn];
		ppn = pbn * PAGES_PER_BLOCK + offset;
		memcpy(&pagebuf[SECTOR_SIZE], &lbn, 4);
		dd_write(pbn * PAGES_PER_BLOCK, pagebuf);

		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		memcpy(&pagebuf[SECTOR_SIZE], &lbn, 4);
		memcpy(&pagebuf[SECTOR_SIZE + 4], &lsn, 4);
		dd_write(ppn, pagebuf);
	}
	free(pagebuf);
	return;
}

void ftl_print()
{
	printf("lbn pbn\n");
	for(int i = 0; i < DATABLKS_PER_DEVICE; i++){
		printf("%d %d\n", i, mapping_table[i]);
	}
	printf("free block's pbn=%d\n", mapping_table[DATABLKS_PER_DEVICE]);
	return;
}

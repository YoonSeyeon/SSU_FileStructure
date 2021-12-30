#ifndef	_PERSON_H_
#define	_PERSON_H_

#define MAX_RECORD_SIZE	100 // including 6 delimeters
#define PAGE_SIZE		256 
#define HEADER_AREA_SIZE	(PAGE_SIZE/10)
#define DATA_AREA_SIZE	(PAGE_SIZE - HEADER_AREA_SIZE)

typedef struct _Person
{
	char id[14];		
	char name[18];	
	char age[4];		
	char addr[22];	    
	char phone[16];		
	char email[26];		
} Person;

#endif

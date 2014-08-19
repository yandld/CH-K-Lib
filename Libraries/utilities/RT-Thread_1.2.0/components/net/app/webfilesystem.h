
#ifndef __WEBFILESYSTEM_H__
#define __WEBFILESYSTEM_H__

#define NUMBER_OF_FILES     50
#define LENGTH_OF_FILENAME	24

typedef enum {
	HTML,
	TEXT,
	GIF,
	JPEG,
	APP
} FileType;

int AddFile(const char filename[LENGTH_OF_FILENAME], FileType filetype, 
			const char *filebegin, int filesize);

int GetFile(char filename[LENGTH_OF_FILENAME], FileType *filetype, 
			const char **filebegin, int *filesize, 
			const char **content,   int *contentsize);

#endif

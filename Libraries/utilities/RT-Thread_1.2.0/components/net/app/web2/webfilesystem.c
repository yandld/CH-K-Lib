
#include <string.h>
#include "webfilesystem.h"

typedef struct {
	char		fileName[LENGTH_OF_FILENAME];
	FileType    fileType;
	const char *fileBegin;
	int			fileSize;
	const char *content;
	int			contentSize;
} WebFile;

static WebFile	webFileSystem[NUMBER_OF_FILES];
static int		fileNumber = 0;

static const char textContent[] = {
	"Content-Type: text/plain\r\n"
};

static const char htmlContent[] = {
	"Content-Type: text/html\r\n"
};

static const char gifContent[]  = {
	"Content-Type: image/gif\r\nCache-Control: max-age 10000\r\n"
};

static const char jpegContent[] = {
	"Content-Type: image/jpeg\r\nCache-Control: max-age 10000\r\n"
};

static const char appContent[]  = {
	"Content-Type: application/octet-stream\r\n"
};

int AddFile(const char filename[LENGTH_OF_FILENAME], FileType filetype, 
			const char *filebegin, int filesize)
{
	int i = fileNumber;

	if ( i < (NUMBER_OF_FILES-1) )
	{
		strcpy(webFileSystem[i].fileName, filename);

		webFileSystem[i].fileType  = filetype;
		webFileSystem[i].fileBegin = filebegin;

		switch (filetype)
		{
			case GIF:
				webFileSystem[i].content	 = gifContent;
				webFileSystem[i].contentSize = sizeof(gifContent)-1;
				webFileSystem[i].fileSize	 = filesize;
				break;

			case HTML:
				webFileSystem[i].content	 = htmlContent;
				webFileSystem[i].contentSize = sizeof(htmlContent)-1;
				webFileSystem[i].fileSize	 = filesize-1;
				break;

			case TEXT:
				webFileSystem[i].content	 = textContent;
				webFileSystem[i].contentSize = sizeof(textContent)-1;
				webFileSystem[i].fileSize	 = filesize-1;
				break;

			case JPEG:
				webFileSystem[i].content	 = jpegContent;
				webFileSystem[i].contentSize = sizeof(jpegContent)-1;
				webFileSystem[i].fileSize	 = filesize;
				break;

			case APP:
				webFileSystem[i].content	 = appContent;
				webFileSystem[i].contentSize = sizeof(appContent)-1;
				webFileSystem[i].fileSize	 = filesize;
				break;

			default:
				return -1;
		}
		fileNumber++;
		return 0;
	}
	return -1;
}

int GetFile(char filename[LENGTH_OF_FILENAME], FileType *filetype, 
			const char **filebegin, int *filesize, 
			const char **content,   int *contentsize)
{
	int i;

	if (filename[0] == '\0' || filename[0] == ' ')
	{
		strcpy(filename, "index.htm");
	}

	for (i = 0; i < fileNumber; i++)
	{
		if (strcmp(filename, webFileSystem[i].fileName) == 0)
		{
			*filetype	 = webFileSystem[i].fileType;
			*filebegin	 = webFileSystem[i].fileBegin;
			*filesize	 = webFileSystem[i].fileSize;
			*content	 = webFileSystem[i].content;
			*contentsize = webFileSystem[i].contentSize;
			return 0;
		}
	}
	return -1;
}

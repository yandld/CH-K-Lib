
#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#define WEB_PORT			80
#define WEB_MAX_CONNECTION	2
#define WEB_BUFFER_SIZE		1024
#define EXPAND_TAG_SIZE     512

int http_parse_variable(const char str[], const char var[], int len, char value[]);

int app_process_post(const char buff[], int len);

int app_process_expand_tag(int tagID, char buff[], int len);

void app_load_webfilesystem(void);

#endif

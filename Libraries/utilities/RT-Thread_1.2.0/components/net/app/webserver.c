
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <rtthread.h>
#include <dfs_posix.h>
#include <lwip/sockets.h>

#include "webfilesystem.h"
#include "webserver.h"

struct web_session
{
	int sockfd;
	struct sockaddr_in remote;
	struct web_session *next;
};

static struct web_session *session_list = NULL;

static struct web_session *web_new_session(void)
{
	struct web_session *session;

	session = (struct web_session*)rt_malloc(sizeof(struct web_session));
	if (session != RT_NULL)
	{
		session->next = session_list;
		session_list = session;
	}
	return session;
}

static void web_close_session(struct web_session *session)
{
	struct web_session *list;

	if (session_list == session)
	{
		session_list = session_list->next;
		session->next = NULL;
	}
	else
	{
		list = session_list;
		while (list->next != session) 
			list = list->next;

		list->next = session->next;
		session->next = NULL;
	}
	rt_free(session);
}

static void http_get_filename(const char buff[LENGTH_OF_FILENAME],
						  char filename[LENGTH_OF_FILENAME])
{
	int i;

	for (i = 0; i < (LENGTH_OF_FILENAME - 1)&& buff[i] != '\0' && buff[i] != ' '; i++)
		filename[i] = buff[i];

	filename[i] = '\0';
}

#define isNumber(n) ((n) >= '0' && (n) <= '9')

static void http_expand_tag(char buff[], const char filebegin[], int *filesize)
{
	int len = *filesize;
	int i = 0, j = 0;
	int tagtotallen = 0;
	int ret;

	if (len > 0)
	{
		while (i < len)
		{
			if (filebegin[i] == '&' &&
				i < (len-2) &&
				isNumber(filebegin[i+1]) &&
				isNumber(filebegin[i+2]) &&
				tagtotallen < EXPAND_TAG_SIZE)
			{
				ret = app_process_expand_tag((filebegin[i+1]-0x30)*10+filebegin[i+2]-0x30,
											&buff[j], EXPAND_TAG_SIZE - tagtotallen);
				if (ret != -1)
				{
					tagtotallen += ret;
					j += ret;
					i += 3;
				}
				else
				{
					buff[j++] = filebegin[i++];
				}
			}
			else
			{
				buff[j++] = filebegin[i++];
			}
		}
		*filesize = j;
	}
}

int http_parse_variable(const char str[], const char var[], int len, char value[])
{
    int i = 0, j = 0;

    while (i < len)
    {
        if (var[j] == '\0')
            break;

        if (str[i] == '=')
        {
            while (str[i] != '&' && i < len)
            {
                i++;
            }
            j = 0;
        }

        if (str[i] == var[j])
        {
            j++;
        }
        else
        {
            j = 0;
        }
        i++;
    }

    if (j == 0)
    {
        return 0;
    }
    else
    {
        i++;
        j = 0;

        while (str[i] != '&' && i < len)
        {
            if (str[i] == '+')
                value[j] = ' ';
            else if (str[i] == '%')
            {
                value[j] = str[i+1] <= '9' ? (str[i+1] - '0')*16:
							(tolower(str[i+1]) - 'a' + 10)*16;
                value[j] += str[i+2] <= '9' ? (str[i+2] - '0'):
                		    (tolower(str[i+2]) - 'a' + 10);
                i += 2;
            }
            else
                value[j] = str[i];
            i++;
            j++;
        }
        value[j] = '\0';
        return 1;
    }
}

static const char status200[] = "HTTP/1.0 200 OK\r\n";

static const char empty_line[] = "\r\n";

static const char page404[] = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
                       "<HTML><HEAD><TITLE>File Not Found</TITLE></HEAD>"
					   "<BODY><H4>The requested page was not found!</H4></BODY>";

static const char page501[] = "HTTP/1.0 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n"
                       "<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD>"
					   "<BODY><H4>The requested method was not implemented!</H4></BODY>";

static int webserver_process_request(const struct web_session *session, char *buff, int len)
{
	int	filenamebegin;

    do
    {
        if (strncmp(buff, "GET", 3) == 0)
        {
			char		filename[LENGTH_OF_FILENAME];
			FileType    filetype;
			const char *filebegin;
			int			filesize;
			const char *content;
			int			contentsize;
			char		*txbuff, *txbuffbase;

			filenamebegin = 5;
SEND_REQUEST_FILE:

			http_get_filename(&buff[filenamebegin], filename);

			if (GetFile(filename, &filetype, &filebegin, &filesize,
					    &content, &contentsize) == 0)
			{
				txbuffbase = txbuff = (char *)rt_malloc(sizeof(status200)+contentsize+
					sizeof(empty_line)+filesize+EXPAND_TAG_SIZE);

				if (txbuffbase)
				{
					memcpy(txbuff, status200, sizeof(status200)-1);
					txbuff += sizeof(status200)-1;

					memcpy(txbuff, content, contentsize);
					txbuff += contentsize;

					memcpy(txbuff, empty_line, sizeof(empty_line)-1);
					txbuff += sizeof(empty_line)-1;

					if (filetype == HTML || filetype == TEXT)
					{
						http_expand_tag(txbuff, filebegin, &filesize);
						txbuff += filesize;
					}
					else
					{				
						memcpy(txbuff, filebegin, filesize);
						txbuff += filesize;
					}
					send(session->sockfd, txbuffbase, txbuff-txbuffbase, 0);
					rt_free(txbuffbase);
				}
			}
			else
			{
				rt_kprintf("GetFile() failed: File %s NO Found!!", filename);
				send(session->sockfd, page404, sizeof(page404)-1, 0);
			}
	    }
		else if (strncmp(buff, "POST", 4) == 0)
		{
			if (app_process_post(buff, len) == 0)
				buff[5] = 0;

			filenamebegin = 6;

			goto SEND_REQUEST_FILE;
		}
		else
		{
			send(session->sockfd, page501, sizeof(page501)-1, 0);
		}
    } while (0);

    return -1;
}

void webserver_thread(void *parameter)
{
	char *buffer;
	int numbytes;
	int sockfd, maxfdp1;
	rt_uint32_t addr_len;
	fd_set readfds, tmpfds;
	struct sockaddr_in local;
	struct web_session *session;

	buffer = (char *)rt_malloc(WEB_BUFFER_SIZE);
	if (buffer == RT_NULL)
		return;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		rt_kprintf("create socket failed\n");
		goto ERROR_HANDLE;
	}

	local.sin_family = PF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(WEB_PORT);
	addr_len = sizeof(struct sockaddr);
	bind(sockfd, (struct sockaddr *)&local, addr_len);

	listen(sockfd, WEB_MAX_CONNECTION);

	FD_ZERO(&readfds);
	FD_ZERO(&tmpfds);
	FD_SET(sockfd, &readfds);

	for (;;)
	{
	    /* get maximum fd */
	    maxfdp1 = sockfd + 1;
        session = session_list;
	    while (session != RT_NULL)
	    {
	        if (maxfdp1 < session->sockfd + 1)
                maxfdp1 = session->sockfd + 1;

            FD_SET(session->sockfd, &readfds);
            session = session->next;
	    }

		tmpfds = readfds;
		if (select(maxfdp1, &tmpfds, 0, 0, 0) == 0) 
			continue;

		if (FD_ISSET(sockfd, &tmpfds))
		{
			int com_socket;
			struct sockaddr_in remote;

			addr_len = sizeof(struct sockaddr);
			com_socket = accept(sockfd, (struct sockaddr*)&remote, &addr_len);
			if (com_socket == -1)
			{
				rt_kprintf("Error on accept()\nContinuing...\n");
				continue;
			}
			else
			{
				rt_kprintf("Got connection from %s\n", inet_ntoa(remote.sin_addr));

				/* new session */
				session = web_new_session();
				if (session != NULL)
				{
					FD_SET(com_socket, &readfds);
					session->sockfd = com_socket;
					session->remote = remote;
				}
				else
				{
					closesocket(com_socket);
				}
			}
		}

		{
			struct web_session *next;

			session = session_list;
			while (session != NULL)
			{
				next = session->next;
				if (FD_ISSET(session->sockfd, &tmpfds))
				{
					memset(buffer, 0, WEB_BUFFER_SIZE);
					numbytes = recv(session->sockfd, buffer, WEB_BUFFER_SIZE, 0);
					if (numbytes == 0 || numbytes == -1)
					{
						rt_kprintf("Client %s disconnected\n",
								inet_ntoa(session->remote.sin_addr));
						FD_CLR(session->sockfd, &readfds);
						closesocket(session->sockfd);
						web_close_session(session);
					}
					else
					{
						buffer[numbytes] = 0;

						//Debug:
						//rt_kprintf("%s requested: total_len=%d:\"%s\"\n",
						//		inet_ntoa(session->remote.sin_addr), numbytes, buffer);

						if (webserver_process_request(session, buffer, numbytes) == -1)
						{
							rt_kprintf("Client %s disconnected\r\n", inet_ntoa(session->remote.sin_addr));
							FD_CLR(session->sockfd, &readfds);
							closesocket(session->sockfd);
							web_close_session(session);
						}
					}
				}
				session = next;
			}
		}
	}
ERROR_HANDLE:
	rt_free(buffer);
}

void webserver_start(void)
{
	rt_thread_t tid;

	rt_kprintf("\n\n\tNow, Initializing The WEB File System...\n");

	app_load_webfilesystem();

	rt_kprintf("\tNow, Starting The WEB Server Thread...\n");

	tid = rt_thread_create("webserver",
		webserver_thread, RT_NULL,
		4096, 30, 5);

	if (tid != RT_NULL)
		rt_thread_startup(tid);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(webserver_start, start web server)
#endif


#include "request.h"
#include "session.h"
#include "mimetype.h"
#include "util.h"
#include "module.h"
#include <stdio.h>

#define DEFAULT_CGI_ROOT        "/cgi-bin/"

// '/'+root+'/'
static char s_cgiroot[16+2] = DEFAULT_CGI_ROOT;

typedef struct _cgi_handle {
    char *name;
    void (*handler)(struct webnet_session* session);
    struct _cgi_handle *next;
} cgi_handle_t;

static cgi_handle_t *s_xCgiTable;

static cgi_handle_t *_cgi_find( const char* name )
{
    cgi_handle_t *p;
    p = s_xCgiTable;
    while(p) {
        if( name && strncasecmp( p->name, name, strlen(name)) == 0 ) {
			return p;
        }
        p=p->next;
    }

    return RT_NULL;
}

static cgi_handle_t *_cgi_insert( const char* name, void (*handler)(struct webnet_session* session) )
{
    cgi_handle_t *p,*q;
    
    p = s_xCgiTable;
    while(p && p->next) {
        p=p->next;
    }
    q = (cgi_handle_t *)rt_malloc( sizeof(cgi_handle_t) );
    if( name ) {
        q->name = rt_strdup( name );
    } else {
        q->name = RT_NULL;
    }
    q->handler = handler;
    q->next = RT_NULL;

    if( p ) {
        p->next = q;
    } else {
        s_xCgiTable = q;
    }

    return q;
}

static void _cgi_del_all( void )
{
    cgi_handle_t *p,*q;
    
    p = s_xCgiTable;
    while(p) {
        q=p->next;
        rt_free( p->name );
        rt_free( p );
        p=q;
    }
    s_xCgiTable = RT_NULL;
}

static int _webnet_module_cgi_init(struct webnet_session* session, int event)
{
    _cgi_del_all();
    
    return WEBNET_MODULE_CONTINUE;
}

static int _webnet_module_cgi_uri_physical(struct webnet_session* session, int event)
{

    // cgi接口不区分大小写
    if( s_xCgiTable && 
        session->request &&
        session->request->path && 
        str_begin_with( session->request->path, s_cgiroot ) ) {
        
        char *name = session->request->path + strlen(s_cgiroot);
        cgi_handle_t *cgi = _cgi_find( name );
        if( cgi && cgi->handler ) {
            cgi->handler( session );
            return WEBNET_MODULE_FINISHED;
        }
    }
    
    return WEBNET_MODULE_CONTINUE;
}

int webnet_module_cgi(struct webnet_session* session, int event)
{
	switch (event)
	{
	case WEBNET_EVENT_INIT:
		return _webnet_module_cgi_init(session, event);
	case WEBNET_EVENT_URI_PHYSICAL:
		return _webnet_module_cgi_uri_physical(session, event);
	case WEBNET_EVENT_URI_POST:
	case WEBNET_EVENT_RSP_HEADER:
	case WEBNET_EVENT_RSP_FILE:
	    return WEBNET_MODULE_CONTINUE;
	default:
		RT_ASSERT(0);
		break;
	}

	return WEBNET_MODULE_CONTINUE;
}

// strlen(root) must < 16
void webnet_cgi_set_root(const char* root)
{
    RT_ASSERT( strlen(root) < (sizeof(s_cgiroot)-2) );
    snprintf( s_cgiroot, sizeof(s_cgiroot), "/%s/", root );
}

// strlen(name) must < 32
void webnet_cgi_register(const char* name, void (*handler)(struct webnet_session* session))
{
    _cgi_insert( name, handler );
}



#include <dmfserver/ws.h>
void wsfunc(connection_tp conn, const Request *req) 
{	
    char key[128] = {0};
    req_get_ws_key(req, key);
    
	upto_ws_prot(conn, key);
}

RouterAdd(ws)
{
	ContFun cf[] = {&wsfunc, NULL};
	char* keys[] = {"/ws", NULL};
	router_add_app(cf, keys, __func__);
}

#include <dmfserver/ws.h>
void wsfunc(int a, const Request *req) 
{	
    char key[128] = {0};
    req_get_ws_key(req, key);
    
	upto_ws_prot(a, key);
}

RouterAdd(ws)
{
	ContFun cf[] = {&wsfunc, NULL};
	char* keys[] = {"/ws", NULL};
	router_add_app(cf, keys, __func__);
}
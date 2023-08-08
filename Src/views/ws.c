

void wsfunc(int a, const Request *req) 
{	
    char key[128] = {0};
    req_get_ws_key(req, key);
    strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    
	upto_ws_prot(a, key);
}

RouterAdd(ws)
{
	ContFun cf[] = {&wsfunc, NULL};
	char* keys[] = {"/ws", NULL};
	router_add_app(cf, keys, __func__);
}
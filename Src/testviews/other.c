
#include <dmfserver/request.h>


#ifdef __WIN32__

void usleep1(unsigned long usec)
{
    HANDLE timer;
    LARGE_INTEGER interval;
    interval.QuadPart = -(10 * usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &interval, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif

void string(int a, const Request* req)
{
	// void *block1 = pool_alloc();
    // usleep1(400);
    // pool_free(block1);
	//printf("%s\n", req->query[0].key);
	res_row(a, "test ok");
}


RouterAdd(other){
	ContFun cf[] = { &string, NULL};
	char* keys[] = { "/string", NULL};
	router_add_app(cf, keys, __func__);
}
#include <dmfserver/model/data.h>
#include <dmfserver/request.h>

void datamodeltest(int a, const Request* req) 
{
	int i = 0;
	objPtr root = CreateObjectNode("root", "hhh");
	objPtr mn1 = CreateObjectNode("Bro0", "ppp");
	objPtr mn2 = CreateObjectNode("Bro1","sss");
	objPtr mn3 = CreateObjectNode("Bro2","sss1");
	objPtr mn4 = CreateObjectNode("Bro3","sss2");
	
	
	AppendChild(root, mn1);
	AppendBro(mn1, mn2);
	AppendBro(mn1, mn3);
	AppendBro(mn1, mn4);


	ShowNodeData(root);
	printf("\n");
	ShowNodeData(mn1);
	printf("\n");

	Res_row(a, "This is a test str");
}

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
	Res_row(a, "test ok");
}


RouterAdd(other){
	ContFun cf[] = {&datamodeltest, &string, NULL};
	char* keys[] = {"/datamodeltest", "/string", NULL};
	router_add_app(cf, keys, __func__);
}
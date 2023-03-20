#include <server.h>


/*



// 多线程实现数据共享的方法
#pragma data_seg("flag_data")
int testcount = 0;
#pragma data_seg()
#pragma comment(linker,"/SECTION:flag_data,RWS")

void set(int a){
	testcount = a;
}
int get(){
	return testcount;
}

*/


void viewtest(int a, const Request* req)
{
	// printf("--in--%s\n", g_server_conf_all._conf_model.database);
	Res_row(a, "lib test ok\n ");
}
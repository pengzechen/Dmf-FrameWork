

void mfunction(char *out, char *in) {
	
	strcpy(out, in);
}

void template(int a, const Request *req) {

	struct Kvmap kv0 = {.key = "name", .value = "Dmfserver", .type = 1};
	struct Kvmap kv1 = {.key = "date", .value = "2023/4/14", .type = 1};
	struct Kvmap kv2 = {.key = "myfunction", .Func = &mfunction, .type = 2};

	struct Kvmap kv[4];
	kv[0] = kv0;
	kv[1] = kv1;
	kv[2] = kv2;

	kv[3].key = "dec1";
	kv[3].dec[0] = "Apache";
	kv[3].dec[1] = "Nginx";
	kv[3].dec[2] = "Http";
	kv[3].dec[3] = NULL;
	kv[3].type = 3;
	
	res_render(a, "test.html", kv, 4);
}

RouterAdd(apptemp)
{
	ContFun cf[] = {&template, NULL};
	char* keys[] = {"/template", NULL};
	router_add_app(cf, keys, __func__);
}
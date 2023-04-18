/* 
Copyright 2023 Ajax

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
*/

#include <dmfserver/utility/utility.h>

int file(){

	FILE *fp;
	fp = fopen("1.png", "rb");
	if(fp == NULL){printf("Open 1.png error"); exit(1);}
	// char ch;
	// while(ch != EOF){
	// ch = fgetc(fp);
	// putchar(ch);
	// }
	long long int n;
	fseek(fp, 0L, 2);
	n = ftell(fp);
	fseek(fp, 0L, 0);
	printf("File size %lld Bytes.\n", n);
	fclose(fp);
}

#ifdef __linux__

char *itoa(int value,char *string,int radix) {

	char zm[37]="0123456789abcdefghijklmnopqrstuvwxyz";
	char aa[100]={0};
	int sum=value;
	char *cp=string;
	int i=0;
	if(radix<2||radix>36)//增加了对错误的检测
	{
		return string;
	}
	if(value<0)
	{
		return string;
	}
	while(sum>0)
	{
		aa[i++]=zm[sum%radix];
		sum/=radix;
	}
	for(int j=i-1;j>=0;j--)
	{
		*cp++=aa[j];
	}
	*cp='\0';
	return string;
}

#endif // linux

void serverTime(char* str)
{
	char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"}; //指针数组-字符串
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep); //取得当地具体时间
    
	// printf("%s, %d/%d/%d %d:%d:%d\n", wday[p->tm_wday], itoa( (1900 + p->tm_year) ), 
			// itoa( (1 + p->tm_mon) ), itoa( p->tm_mday), itoa( p->tm_hour), itoa( p->tm_min), itoa( p->tm_sec) );
			
	char time_str[10] = {'\0'};
	
	// strcat( str, wday[p->tm_wday] );
	// strcat( str, ",");
	
	itoa( (1900 + p->tm_year), time_str, 10 );
	strcat(str, time_str );
	strcat( str, "/");
	
	itoa( (1 + p->tm_mon), time_str, 10 );
	strcat(str, time_str);
	strcat( str, "/");
	
	itoa( p->tm_mday, time_str, 10 );
	strcat(str, time_str);
	strcat( str, " ");
	
	itoa( p->tm_hour, time_str, 10 );
	strcat(str, time_str);
	strcat( str, ":");
	
	itoa( p->tm_min, time_str, 10 );
	strcat(str, time_str);
	strcat( str, ":");
	
	itoa( p->tm_sec, time_str, 10 );
	strcat(str, time_str);
}
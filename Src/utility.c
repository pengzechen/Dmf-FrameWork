
#include "utility.h"

int file(){

	FILE *fp;
	fp = fopen("1.png", "rb");
	if(fp == NULL){printf("Open 1.png error"); exit(1);}
	// char ch;
	// while(ch != EOF){
	// ch = fgetc(fp);
	// putchar(ch);
	// }
	unsigned long int n;
	fseek(fp, 0L, 2);
	n = ftell(fp);
	fseek(fp, 0L, 0);
	printf("File size %d Bytes.\n", n);
	fclose(fp);
}


void serverTime(char* str){
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
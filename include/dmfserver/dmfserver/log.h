#ifndef __LOG__
#define __LOG__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>


// 定义日志级别
#define LOG_LEVEL_INFO  0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

// 定义日志格式
#define LOG_FORMAT "[%d] %s %s:%d\n"

// 定义日志文件名和大小
#define LOG_FILE_NAME "server.log"
#define LOG_FILE_SIZE (1024 * 1024 * 10)  // 10MB

// 定义日志缓冲区大小
#define LOG_BUFFER_SIZE 1024

// 定义日志记录
typedef struct {
    int level;              // 日志级别
    char message[1024];     // 日志内容
    char filename[256];     // 日志文件名
    int line;               // 日志所在行数
} log_record_t;


#ifdef __cplusplus
extern "C" {
#endif

    static void *log_thread(void *arg);
    
    extern void log_init();
    
    extern void log_write(int level, const char *filename, int line, const char *format, ...);
    
    extern void log_info(const char *filename, int line, const char *format, ...);
    
    extern void log_debug(const char *filename, int line, const char *format, ...);
    
    extern void log_warn(const char *filename, int line, const char *format, ...);
    
    extern void log_error(const char *filename, int line, const char *format, ...);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif  // __LOG__
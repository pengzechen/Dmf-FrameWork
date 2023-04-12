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

#include <dmfserver/log.h>



// 定义日志缓冲区
static log_record_t log_buffer[LOG_BUFFER_SIZE];
static int log_buffer_count = 0;

// 定义日志写入锁和条件变量
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t log_cond = PTHREAD_COND_INITIALIZER;

// 定义日志写入线程
static void *log_thread(void *arg) 
{
    FILE *fp = NULL;
    log_record_t record;

    // 打开日志文件
    fp = fopen(LOG_FILE_NAME, "a");
    if (fp == NULL) {
        return NULL;
    }
    while (1) {
        // 加锁
        pthread_mutex_lock(&log_mutex);
        // 等待条件变量
        while (log_buffer_count <= 0) {
            pthread_cond_wait(&log_cond, &log_mutex);
        }
        // 从缓冲区中取出一条日志记录
        memcpy(&record, &log_buffer[0], sizeof(log_record_t));
        log_buffer_count--;
        // 将日志记录写入文件
		fprintf(fp, LOG_FORMAT, record.level, record.message, record.filename, record.line);
		// 刷新缓冲区
		fflush(fp);
		// 解锁
		pthread_mutex_unlock(&log_mutex);
	}
	// 关闭日志文件
	fclose(fp);
	return NULL;
}

// 定义日志初始化函数
void log_init() 
{
	// 创建日志写入线程
	pthread_t tid;
	pthread_create(&tid, NULL, log_thread, NULL);
	// 分离日志写入线程
	pthread_detach(tid);

    printf("[Log: Info] log start \n");
}

// 定义日志记录函数
void log_write(int level, const char *filename, int line, const char *format, ...) 
{
	va_list args;
	char message[1024];
	// 格式化日志内容
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);
	// 加锁
	pthread_mutex_lock(&log_mutex);
	// 如果缓冲区已满，则等待条件变量
	while (log_buffer_count >= LOG_BUFFER_SIZE) {
		pthread_cond_wait(&log_cond, &log_mutex);
	}
	// 将日志记录添加到缓冲区中
	log_record_t *record = &log_buffer[log_buffer_count++];
	record->level = level;
	strcpy(record->message, message);
	strcpy(record->filename, filename);
	record->line = line;
	// 唤醒条件变量
	pthread_cond_signal(&log_cond);
	// 解锁
	pthread_mutex_unlock(&log_mutex);
}

// 定义日志信息函数
void log_info(const char *filename, int line, const char *format, ...) 
{
	va_list args;
	char message[1024];
	// 格式化日志内容
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);
	// 写入日志
	log_write(LOG_LEVEL_INFO, filename, line, message);
}

// 定义日志调试函数
void log_debug(const char *filename, int line, const char *format, ...) 
{
	va_list args;
	char message[1024];
	// 格式化日志内容
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	// 写入日志
	log_write(LOG_LEVEL_DEBUG, filename, line, message);
}

// 定义日志警告函数
void log_warn(const char *filename, int line, const char *format, ...) 
{
	va_list args;
	char message[1024];
	// 格式化日志内容
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	// 写入日志
	log_write(LOG_LEVEL_WARN, filename, line, message);
}

// 定义日志错误函数
void log_error(const char *filename, int line, const char *format, ...) 
{
	va_list args;
	char message[1024];
	// 格式化日志内容
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	// 写入日志
	log_write(LOG_LEVEL_ERROR, filename, line, message);
}


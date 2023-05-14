/* 
    *  Copyright 2023 Ajax
    *
    *  Licensed under the Apache License, Version 2.0 (the "License");
    *  you may not use this file except in compliance with the License.
    *
    *  You may obtain a copy of the License at
    *
    *    http://www.apache.org/licenses/LICENSE-2.0
    *    
    *  Unless required by applicable law or agreed to in writing, software
    *  distributed under the License is distributed on an "AS IS" BASIS,
    *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    *  See the License for the specific language governing permissions and
    *  limitations under the License. 
    *
    */

/*  
    *                       MASTER FOR SERVER
    *
    *   This model is for linux, it start a daemon process and some worker processes
    *   when a worker process exit unexpectedly, daemon process will restart a new
    *   worker processs.
    */


#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

// define default workers num
#define DEFAULT_WORKER_NUM 2
// save workers pid
static pid_t worker[DEFAULT_WORKER_NUM];
// define function pointer
typedef void (*worker_function)();

volatile bool is_running2 = true;

static worker_function wf;


void start_worker()
{
    pid_t pid;

    for(int i=0; i<DEFAULT_WORKER_NUM; i++){
        // start worker process
        pid = fork();
        if (pid == 0) {

            wf();

        } else if (pid < 0){
            perror("fork failed!");
        } else {
            // father process
            worker[i] = pid;
        }
    }
}

// set daemon
static void daemonize()
{
    pid_t pid = fork();

    if (pid < 0) { 
        perror("fork"); exit(1); 
    }
    else if (pid > 0) {

        printf("MultiProcess Serer Running...\nDaemon PID: %d\n", pid);
        exit(0);
    }
    
    if (setsid() < 0) {
        perror("setsid"); exit(1);
    }

    umask(0);

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDWR);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDONLY);

}


void signal_handle(int signum){

    if(signum == SIGUSR1) {
        is_running2 = false;
        for(int i=0; i<DEFAULT_WORKER_NUM; i++)
            kill(worker[i], SIGTERM);
    }
}


void check_and_restart()
{
    int i, status;
    pid_t pid;

    while(is_running2) {

        for(i=0; i<DEFAULT_WORKER_NUM; i++){

            pid = waitpid(worker[i], &status, WNOHANG);
            if(pid == 0) {

                // worker is running

            } else if (pid == worker[i]) {
                // worker is down
                pid = fork();

                if (pid < 0) {
                    perror("fork failed!");
                    exit(1);
                } else if (pid == 0) {
                    wf();
                    exit(0);
                } else {
                    // father
                    printf("Worker %d has been down, ", worker[i]);
                    worker[i] = pid;
                    printf("Start new worker %d \n", pid);
                }
            } else {
                perror("waitpid failed!");
                exit(1);
            }
        }
        sleep(1);
    }
}


void multi_process_init(worker_function _wf)
{
    wf = _wf;

    // kill -10 pid
    signal(SIGUSR1, signal_handle);

    daemonize();

    start_worker();

    check_and_restart();

}


// worker function
void myworker()
{
    sleep(100);
}


int main2() {
    multi_process_init(&myworker);
}
#endif // __linux__
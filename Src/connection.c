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
#include <dmfserver/socket.h>
#include <dmfserver/connection.h>
#include <dmfserver/cfg.h>
#include <dmfserver/mpool.h>


// 仅仅做分配工作
extern connection_tp 
new_connection () {
    connection_tp conn_ptr = (connection_tp)malloc(sizeof(connection_t));
#ifdef __SERVER_MPOOL__
    conn_ptr-> per_handle_data =  (per_handle_data_t*)pool_alloc2();
    conn_ptr-> per_io_data = (per_io_data_t*)pool_alloc();
#else
    conn_ptr->per_handle_data =  (per_handle_data_t*)malloc(sizeof(per_handle_data_t));
    // 建立一个Overlapped，并使用这个Overlapped结构对socket投递操作
    conn_ptr->per_io_data  =  (per_io_data_t*)malloc(sizeof(per_io_data_t));     
#endif // __SERVER_MPOOL__
    conn_ptr->req = (Request*)malloc(sizeof(Request));
    
    return conn_ptr;
}

extern void 
connection_close (connection_tp conn) {
#ifdef __linux__
    epoll_ctl(conn->per_handle_data->efd, 2, 
        conn->per_handle_data->Socket, NULL);  // EPOLL_CTL_DEL 2
#endif
	close_socket(conn->per_handle_data->Socket);
}

#ifdef __WIN32__
extern void
send_next (connection_tp conn) {
    // 继续向 socket 投递WSARecv操作
    DWORD Flags = 0;
    DWORD dwRecv = 0;
    ZeroMemory( conn->per_io_data, sizeof(per_io_data_t) );
    conn->per_io_data->DataBuf.buf = conn->per_io_data->Buffer;
    conn->per_io_data->DataBuf.len = DATA_BUFSIZE;
    WSARecv( conn->per_handle_data->Socket, &conn->per_io_data->DataBuf, 1, &dwRecv, 
            &Flags, &conn->per_io_data->Overlapped, NULL);
}
#endif

extern void
connection_free_base (connection_tp conn) {
#ifdef __SERVER_MPOOL__
    pool_free( conn->per_io_data );
    pool_free2( conn->per_handle_data );
#else 
    free( conn->per_io_data);
    free( conn->per_handle_data);
#endif // __SERVER_MPOOL__
}

extern void
connection_free (connection_tp conn) {
    req_free(conn->req);
    free(conn->req);

#ifdef __SERVER_MPOOL__
    pool_free( conn->per_io_data );
    pool_free2( conn->per_handle_data );
#else 
    free( conn->per_io_data );
    free( conn->per_handle_data );
#endif // __SERVER_MPOOL__
    free(conn);
}

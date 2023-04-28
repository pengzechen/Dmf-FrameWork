# Diff Details

Date : 2023-04-28 10:42:18

Directory c:\\Users\\92706\\Desktop\\TEMP\\C_learn\\Myserver\\Dmfserver\\request

Total : 57 files,  387 codes, 202 comments, 124 blanks, all 713 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [README.md](/README.md) | Markdown | 2 | 0 | 2 | 4 |
| [Src/conf/conf.c](/Src/conf/conf.c) | C | 0 | 1 | 2 | 3 |
| [Src/conf/test.c](/Src/conf/test.c) | C | 0 | 1 | 0 | 1 |
| [Src/cpool.c](/Src/cpool.c) | C | 0 | 8 | 2 | 10 |
| [Src/link/link.c](/Src/link/link.c) | C | -93 | -24 | -19 | -136 |
| [Src/link/linktest.c](/Src/link/linktest.c) | C | -21 | -17 | -12 | -50 |
| [Src/log.c](/Src/log.c) | C | 0 | 8 | 1 | 9 |
| [Src/main.c](/Src/main.c) | C | -8 | 17 | -6 | 3 |
| [Src/master.c](/Src/master.c) | C | 0 | 24 | 3 | 27 |
| [Src/mdb/libshare.c](/Src/mdb/libshare.c) | C | 93 | 25 | 20 | 138 |
| [Src/mdb/mdb3.c](/Src/mdb/mdb3.c) | C | 66 | 18 | 20 | 104 |
| [Src/mdb/mdb_operate.c](/Src/mdb/mdb_operate.c) | C | 58 | 19 | 16 | 93 |
| [Src/mdb/mdbrw3.c](/Src/mdb/mdbrw3.c) | C | 101 | 19 | 25 | 145 |
| [Src/mdb/share.c](/Src/mdb/share.c) | C | 37 | 17 | 6 | 60 |
| [Src/model/data.c](/Src/model/data.c) | C | 0 | 1 | 0 | 1 |
| [Src/model/model.c](/Src/model/model.c) | C | 6 | 1 | 0 | 7 |
| [Src/mpool.c](/Src/mpool.c) | C | 76 | 11 | 16 | 103 |
| [Src/request.c](/Src/request.c) | C | 0 | 7 | 2 | 9 |
| [Src/response.c](/Src/response.c) | C | -1 | 6 | 4 | 9 |
| [Src/router.c](/Src/router.c) | C | 0 | 1 | 1 | 2 |
| [Src/server.c](/Src/server.c) | C | 24 | 7 | 16 | 47 |
| [Src/session.c](/Src/session.c) | C | 0 | 1 | 2 | 3 |
| [Src/template.c](/Src/template.c) | C | 22 | 2 | -3 | 21 |
| [Src/utility/dm_list.c](/Src/utility/dm_list.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/dm_queue.c](/Src/utility/dm_queue.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/dm_stack.c](/Src/utility/dm_stack.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/dm_string.c](/Src/utility/dm_string.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/dm_thread_pool.c](/Src/utility/dm_thread_pool.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/dm_timer.c](/Src/utility/dm_timer.c) | C | 0 | 1 | 0 | 1 |
| [Src/utility/utility.c](/Src/utility/utility.c) | C | 0 | 1 | 0 | 1 |
| [Src/views/mdb.c](/Src/views/mdb.c) | C | 14 | 0 | 6 | 20 |
| [Src/views/mysql.c](/Src/views/mysql.c) | C | 3 | 2 | 0 | 5 |
| [Src/views/other.c](/Src/views/other.c) | C | -2 | 2 | 1 | 1 |
| [Src/views/template.c](/Src/views/template.c) | C | -3 | 0 | -3 | -6 |
| [bin/conf.xml](/bin/conf.xml) | XML | -27 | 0 | -6 | -33 |
| [include/dmfserver/dmfserver/conf/conf.h](/include/dmfserver/dmfserver/conf/conf.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/cpool.h](/include/dmfserver/dmfserver/cpool.h) | C++ | 0 | 1 | 2 | 3 |
| [include/dmfserver/dmfserver/link.h](/include/dmfserver/dmfserver/link.h) | C | -28 | -16 | -15 | -59 |
| [include/dmfserver/dmfserver/log.h](/include/dmfserver/dmfserver/log.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/mdb/mdb.h](/include/dmfserver/dmfserver/mdb/mdb.h) | C++ | 27 | 17 | 11 | 55 |
| [include/dmfserver/dmfserver/mdb/share.h](/include/dmfserver/dmfserver/mdb/share.h) | C++ | 28 | 17 | 16 | 61 |
| [include/dmfserver/dmfserver/model/data.h](/include/dmfserver/dmfserver/model/data.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/model/model.h](/include/dmfserver/dmfserver/model/model.h) | C++ | -3 | 1 | 0 | -2 |
| [include/dmfserver/dmfserver/mpool.h](/include/dmfserver/dmfserver/mpool.h) | C++ | 3 | 1 | 0 | 4 |
| [include/dmfserver/dmfserver/request.h](/include/dmfserver/dmfserver/request.h) | C++ | 0 | 1 | 2 | 3 |
| [include/dmfserver/dmfserver/response.h](/include/dmfserver/dmfserver/response.h) | C++ | 0 | 1 | 1 | 2 |
| [include/dmfserver/dmfserver/router.h](/include/dmfserver/dmfserver/router.h) | C++ | 0 | 1 | 1 | 2 |
| [include/dmfserver/dmfserver/server.h](/include/dmfserver/dmfserver/server.h) | C++ | 1 | 2 | 2 | 5 |
| [include/dmfserver/dmfserver/session.h](/include/dmfserver/dmfserver/session.h) | C++ | 0 | 1 | 1 | 2 |
| [include/dmfserver/dmfserver/template.h](/include/dmfserver/dmfserver/template.h) | C++ | 12 | 1 | 6 | 19 |
| [include/dmfserver/dmfserver/utility/dm_list.h](/include/dmfserver/dmfserver/utility/dm_list.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/dm_queue.h](/include/dmfserver/dmfserver/utility/dm_queue.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/dm_stack.h](/include/dmfserver/dmfserver/utility/dm_stack.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/dm_string.h](/include/dmfserver/dmfserver/utility/dm_string.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/dm_thread_pool.h](/include/dmfserver/dmfserver/utility/dm_thread_pool.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/dm_timer.h](/include/dmfserver/dmfserver/utility/dm_timer.h) | C++ | 0 | 1 | 0 | 1 |
| [include/dmfserver/dmfserver/utility/utility.h](/include/dmfserver/dmfserver/utility/utility.h) | C++ | 0 | 1 | 1 | 2 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details
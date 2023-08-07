# Dmf-FrameWork

This project is a cross-platform web server framework implemented in C language,  
supporting Linux and Windows operating systems, with the following features:

- Adopts the traditional MVC pattern 
- Implements HTTP template functionality 
- Implements session functionality 
- Can be extended through middleware 
- Implements IP restriction functionality, adding the same IP to a blacklist if it frequently initiates requests 
- Supports asynchronous logging and IP monitoring middleware 
- Achieves a simple template HTTP request concurrency of up to 5600/sec(2023/5/8)

## Technology Stack

- Multi-threading
- IO multiplexing
- Thread pool
- Connection pool
- Shared memory
- Integrated SSL and MySQL

## Middleware
This framework supports extension through middleware. Currently, we have implemented the following middleware:

- IP restriction middleware
- Asynchronous logging middleware
- IP monitoring middleware

## Stress test results

![result1](https://github.com/pengzechen/request/blob/master/doc/2023-05-12%20(2).png "res1")
![result3](https://github.com/pengzechen/request/blob/master/doc/2023-05-12%20(1).png "res3")


## To run this server:

#### 1.Install MySql

You need to Install MySql first, then Create user , and Create a database.  
Here is example:
```
create database dbtest charset=utf8;
create user 'pzc'@'localhost' identified by 'pzc123';
grant all privileges on dbtest.* to 'pzc'@'localhost';
plush privileges;
```
Pay attention: I used MySql(5.7), if you use aother version of MySql, please replace "include/mysql" and "lib/libmysql.lib". Don't forget to copy the libmysql.dll to "/bin".

#### 2.Clone and Compile the project

```
git clone https://github.com/pengzechen/request
cd request
mkdir build
cd build
cmake ../CMakeLists.txt -G "MinGW Makefiles"
make
```
Pay attention: You need to have MinGW and it need to be in PATH
Or you can Install Vistual Studio 2019


#### 3.Configure "conf.xml"
Here is example:

```
<?xml version="1.0" encoding="gbk"?>
<dmfserver>
  <server>
	  <listen>80</listen>
  </server>
  <model>
    <host>localhost</host>
    <port>3306</port>
    <username>pzc</username>
    <password>pzc123</password>
    <database>dbtest</database>
  </model>
  <template>
	  <dir>./template</dir>
  </template>
  <static>
	  <dir>./static</dir>
  </static>
</dmfserver>
```

#### 4.Run Server
```
cd ../bin
./server
```

#### 5.Linux Configure
```
apt-get install -y libmysqlclient-dev libssl-dev libxml2-dev

```

## Contribution
Contributions to this project are welcome. If you wish to contribute code, please email to me.

## License
This project is licensed under the Apache license.


## Some useful command lines

git config --global http.proxy 'socks5://127.0.0.1:1080'

netsh int ipv4 set dynamicport tcp start=1025 num=64500

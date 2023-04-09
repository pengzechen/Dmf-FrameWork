# Dmfserver

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

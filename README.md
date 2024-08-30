# Introduction
uses libhv to implement tcp client and server, tcp client support sync and async message sending

## Tcp client async message sending
```c++
client.SendSyncMessage(message)
```
## Tcp client sync message sending
```c++
client.SendMessage(message)
```

# Build
```shell
mkdir build
cd build
cmake ..
make
```

# Running
> uses default port 9080
## Server
```shell
./libhv_server
```

## Client
```shell
./libhv_client
```
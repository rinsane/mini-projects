1. Make sure you have the latest version of openssl installed in your system:
```
sudo apt-get install libssl-dev
```

2. Compile the key_server.c, client.c and server.c using the follwing commands:

```
gcc -o key_server key_server.c -lssl -lcrypto -lpthread
gcc -o server server.c -lssl -lcrypto -lpthread
gcc -o client client.c -lssl -lcrypto -lpthread
```

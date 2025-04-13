To compile the code, you need to type the following commands:

1. Compile the `aes.c` file without linking:
```bash
gcc -c aes.c
```

2. For Server:
```bash
gcc -o server_chat_2 server_chat_2.c aes.o
```

3. For Client:
```bash
gcc -o client_chat_2 client_chat_2.c aes.o
```

4. Run Server side in one window:
```bash
./server_chat_2
```

5. Run Client side in one window:
```bash
./client_chat_2
```

6. Enjoy safe chatting!

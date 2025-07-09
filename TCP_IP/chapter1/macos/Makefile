CC = gcc
CFLAGS = -Wall -Wextra -pedantic # 不忽略警告，开启更多检查

hello : server client

server: hello_server.c
	$(CC) $(CFLAGS) -o server hello_server.c

client: hello_client.c
	$(CC) $(CFLAGS) -o client hello_client.c

clean:
	rm server client

.PHONY: clean
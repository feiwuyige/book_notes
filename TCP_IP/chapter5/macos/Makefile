CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic # 不忽略警告，开启更多检查

compute: server client

server: computer_server.c
	$(CC) $(CFLAGS) -o server computer_server.c

client: computer_client.c
	$(CC) $(CFLAGS) -o client computer_client.c

clean:
	rm server client

.PHONY: clean

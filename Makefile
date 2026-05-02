CC = gcc # compiler
CFLAGS = -Wall -Wextra -std=c11
TARGET = slim
SRC = src/slim.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
clean:
	rm -f $(TARGET)
test:
	@echo 'int main() { return undefined_var;}' > /tmp/test_broken.c
	-./slim gcc /tmp/test_broken.c -o /tmp/test_broken_out
	@rm -f /tmp/test_broken.c /tmp/test_broken_out

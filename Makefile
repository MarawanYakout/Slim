CC = gcc # compiler
CFLAGS = -Wall -Wextra -std=c11
TARGET = slim
SRC = src/slim.c

INSTALL_DIR = /usr/local/lib/slim
BIN_DIR = /usr/local/bin

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
clean:
	rm -f $(TARGET)

install:
	sudo mkdir -p $(INSTALL_DIR)
	sudo cp $(TARGET)		$(INSTALL_DIR)/slim_interceptor
	sudo cp bridge/ask_llm.py	$(INSTALL_DIR)/ask_llm.py
	sudo cp scripts/slim		$(BIN_DIR)/slim
	sudo chmod +x			$(BIN_DIR)/slim
	@echo "Slim Installed, Try: slim gcc file.c" #temporary for now
uninstall:
	sudo rm -rf $(INSTALL_DIR)
	sudo rm -f  $(BIN_DIR)/slim
	@echo "Slim is Uninstalled"
test:
	@echo 'int main() { return undefined_var;}' > /tmp/test_broken.c
	-./slim gcc /tmp/test_broken.c -o /tmp/test_broken_out
	@rm -f /tmp/test_broken.c /tmp/test_broken_out

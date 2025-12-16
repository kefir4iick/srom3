CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = gf293_test

all: $(TARGET)

$(TARGET): main.c gf293.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c gf293.c

clean:
	rm -f $(TARGET) *.o

test: $(TARGET)
	./$(TARGET)

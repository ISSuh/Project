CC = gcc
CFLAGS = -W -Wall -D _REENTRANT -lpthread
TARGET = Client_test
OBJECTS = Multi_Client.o
SOURCE = Multi_Client.c

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

clean :
	rm *.o Client_test

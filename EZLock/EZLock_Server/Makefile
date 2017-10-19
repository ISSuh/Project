CC = gcc
CFLAGS = -W -Wall -D _REENTRANT -lpthread -lmysqlclient
TARGET = EZLock_Server
OBJECTS = EZLock_Server.o
SOURCE = EZLock_Server.c

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

clean :
	rm *.o EZLock_Server

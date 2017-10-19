CC = gcc
CFLAGS = -W -Wall -D _REENTRANT -lpthread -lmysqlclient
TARGET = Server_test
OBJECTS = Multi_Server.o
SOURCE = Multi_Server.c

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

clean :
	rm *.o Server_test

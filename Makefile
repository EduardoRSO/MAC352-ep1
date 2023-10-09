CC = gcc
DOTCS = src/main.c src/amqp.c src/queue.c src/hardcode.c
DOTHS = src/amqp.h src/queue.h src/hardcode.h
TARGET = main

$(TARGET): $(DOTCS) $(DOTHS)
	$(CC) -o $(TARGET) $(DOTCS)



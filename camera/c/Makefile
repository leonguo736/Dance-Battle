CC = gcc
CFLAGS = -std=gnu99 -pthread
LIBS = -lintelfpgaup -lm

SRCS = main_camera.c display.c camera.c
OBJS = $(SRCS:.c=.o)
TARGET = main_camera

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -MMD -MP
TARGET  = ime
SRCS    = core/main.c core/dynbuf.c
OBJS    = $(SRCS:.c=.o)
DEPS    = $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
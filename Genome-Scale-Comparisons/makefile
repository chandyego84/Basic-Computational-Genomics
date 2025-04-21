CC = gcc
CFLAGS = -Wall -g
 
TARGET = similarity_matrix

SRCS = main.c input_parser.c similarity.c suffix_tree.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del -f $(OBJS) $(TARGET)

.PHONY: all clean
CC = gcc
CFLAGS = -Wall -g

TARGET = suffix_tree

SRCS = main.c input_parser.c
OBJS = $(SRCS:.c=.o)

# Default target (build the executable)
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to create object files from C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object and executable files
clean:
	del -f $(OBJS) $(TARGET)

# Rebuild everything
rebuild: clean all

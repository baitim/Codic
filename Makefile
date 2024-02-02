FILE_PATHS = $(wildcard *.c)
FILES = $(FILE_PATHS)
OBJECTS_DIR = build
OBJECTS_NAMES = $(FILES:.c=.o)
OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(OBJECTS_NAMES))

CC = gcc

.PHONY : clean

all : server

server : $(OBJECTS)
	@$(CC) -o server $(OBJECTS)

$(OBJECTS) : $(OBJECTS_DIR)/%.o: %.c
	@$(CC) -c $< -o $@

clean :
	rm server $(OBJECTS)
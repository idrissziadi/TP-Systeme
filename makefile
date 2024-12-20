CC = gcc
CFLAGS = -Wall

# Executables
MAIN = main
PROCESS = Pgme_processus

# Source Files
MAIN_SRC = main.c semaphore.c segment.c
PROCESS_SRC = processus.c semaphore.c segment.c

# Object Files
MAIN_OBJ = $(MAIN_SRC:.c=.o)
PROCESS_OBJ = $(PROCESS_SRC:.c=.o)

# Targets
all: $(MAIN) $(PROCESS)

$(MAIN): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN_SRC)

$(PROCESS): $(PROCESS_SRC)
	$(CC) $(CFLAGS) -o $(PROCESS) $(PROCESS_SRC)

clean:
	rm -f $(MAIN) $(PROCESS) *.o

run:
	./$(MAIN)

.PHONY: all clean run

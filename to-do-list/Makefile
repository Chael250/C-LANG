# Object files
OBJS = main.o task.o file.o utils.o

# Target executable
TARGET = todo-list

# Compile all
all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $(TARGET)

main.o: main.c task.h file.h utils.h
	gcc -c main.c -o main.o

task.o: task.c task.h
	gcc -c task.c -o task.o

file.o: file.c file.h
	gcc -c file.c -o file.o

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

clean:
	rm -f *.o $(TARGET)
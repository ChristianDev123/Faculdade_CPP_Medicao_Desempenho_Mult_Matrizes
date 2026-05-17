CC = g++
SOURCE = main

all: clean $(SOURCE)
	./$(SOURCE).o
$(SOURCE):
	$(CC) $(SOURCE).cpp -o $(SOURCE).o
clean:
	rm -rf *.o
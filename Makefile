build: LinkedList.o HashTable.o main.o
	gcc $^ -o so-cpp

main.o: main.c
	gcc -c $^

LinkedList.o: LinkedList.h LinkedList.c
	gcc -c $^

HashTable.o: HashTable.c HashTable.h
	gcc -c $^

clean:
	rm *.o so-cpp *.gch
build: LinkedList.o HashTable.o main.o utils.h
	gcc -g $^ -o so-cpp

main.o: main.c
	gcc -g -c $^

LinkedList.o: LinkedList.h LinkedList.c
	gcc -c $^

HashTable.o: HashTable.c HashTable.h
	gcc -c $^

clean:
	rm *.o so-cpp *.gch
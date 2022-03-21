CFLAGS = /D_CRT_SECURE_NO_DEPRECATE /W3 /EHsc /Za /MD /D_NO_CRT_STDIO_INLINE /NODEFAULTLIB:LIBCMT

build: so-cpp.obj HashTable.obj LinkedList.obj
	cl.exe $(CFLAGS) /Feso-cpp.exe so-cpp.obj HashTable.obj LinkedList.obj

so-cpp.obj: main.c     
	cl.exe $(CFLAGS) /Foso-cpp.obj /c main.c

HashTable.obj: HashTable.c     
	cl.exe $(CFLAGS) /FoHashTable.obj /c HashTable.c

LinkedList.obj: LinkedList.c     
	cl.exe $(CFLAGS) /FoLinkedList.obj /c LinkedList.c

clean:
	del *.obj *.exe
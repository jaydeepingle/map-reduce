#
# Makefile
# jaydeep, 2017-09-20 19:42
#

all: memory_manager.a

memory_manager.a: memory_manager.o bitmap.o	
	ar rc memory_manager.a memory_manager.o bitmap.o

memory_manager.o: memory_manager.c memory_manager.h common.h 
	gcc -g -c memory_manager.c

bitmap.o: bitmap.c common.h 
	gcc -g -c bitmap.c

clean:
	rm -f *.o *.a

test:
	gcc -g memory_manager.c bitmap.c test_main.c

run:
	./a.out

memcheck:
	valgrind -v --tool=memcheck --track-origins=yes ./a.out

# vim:ft=make
#
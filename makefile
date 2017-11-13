all: svr_c svr_s clean

svr_c: svr_c.o
	gcc -o svr_c svr_c.o

svr_c.o: svr_c.c
	gcc -c svr_c.c

svr_s: svr_s.o utilities.o utilities.h
	gcc -Wall -Wextra -g -o svr_s svr_s.o utilities.o

svr_s.o: svr_s.c utilities.h
	gcc -Wall -Wextra -g -c svr_s.c

utilities.o: utilities.c utilities.h
	gcc -Wall -Wextra -g -c utilities.c

clean:
		rm -f *.o
		rm -f *gch

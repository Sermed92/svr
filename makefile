all: svr_c svr_s clean

utilities.o: utilities.c utilities.h
	gcc -Wall -Wextra -std=c99 -g -c utilities.c

svr_c: svr_c.o utilities.o utilities.h
	gcc -pthread -Wall -std=c99 -Wextra -g -o svr_c svr_c.o utilities.o

svr_c.o: svr_c.c utilities.h
	gcc -Wall -Wextra -std=c99 -g -c svr_c.c

svr_s: svr_s.o utilities.o utilities.h
	gcc -pthread -Wall -std=c99 -Wextra -g -o svr_s svr_s.o utilities.o

svr_s.o: svr_s.c utilities.h
	gcc -Wall -Wextra -std=c99 -g -c svr_s.c

clean:
		rm -f *.o
		rm -f *gch

.PHONY: all
all: exec

.PHONY: exec
exec: exec.o reading.o
	g++ -o exec exec.o reading.o
.PHONY: reading.o
reading.o: reading.h
	g++ -c -o reading.o reading.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: exec.o
exec.o: reading.h
	g++ -c -o exec.o exec.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: clean
clean:
	rm -f *.o reading a.out

.PHONY: all
all: exec

.PHONY: exec
exec: exec.o reading.o
	g++ -std=c++11 -o exec exec.o reading.o

.PHONY: KDTree.o
KDTree.o: KDTree.h
	g++ -std=c++11 -c -o KDTree.o KDTree.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: reading.o
reading.o: reading.h KDTree.h
	g++ -std=c++11 -c -o reading.o reading.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: exec.o
exec.o: reading.h
	g++ -std=c++11 -c -o exec.o exec.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: clean
clean:
	rm -f *.o reading a.out

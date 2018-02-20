.PHONY: all
all: exec

.PHONY: exec
exec: exec.o reading.o point.o KDTree.o
	g++ -std=c++11 -pthread -g -o exec exec.o reading.o point.o KDTree.o

.PHONY: point.o
point.o: point.h
	g++ -std=c++11 -pthread -g -c -o point.o point.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: KDTree.o
KDTree.o: KDTree.h
	g++ -std=c++11 -pthread -g -c -o KDTree.o KDTree.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: reading.o
reading.o: reading.h KDTree.h point.h
	g++ -std=c++11 -pthread -g -c -o reading.o reading.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: exec.o
exec.o: reading.h point.h
	g++ -std=c++11 -pthread -g -c -o exec.o exec.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: clean
clean:
	rm -f *.o reading a.out exec

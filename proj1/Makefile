all: exec

exec: exec.o reading.o point.o KDTree.o
	g++ -O3 -std=c++17 -pthread -g -o exec exec.o reading.o point.o KDTree.o

point.o: point.h
	g++ -std=c++17 -pthread -g -c -o point.o point.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

KDTree.o: KDTree.h
	g++ -std=c++17 -pthread -g -c -o KDTree.o KDTree.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

reading.o: reading.h KDTree.h point.h
	g++ -std=c++17 -pthread -g -c -o reading.o reading.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

exec.o: reading.h point.h
	g++ -std=c++17 -pthread -g -c -o exec.o exec.cpp -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align

.PHONY: clean
clean:
	rm -f *.o reading a.out exec

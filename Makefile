CC = g++
Arg = -std=c++0x
Target = main

${Target} : main.o ThreadPool.o
	${CC} ${Arg} -o ${Target} main.o ThreadPool.o

main.o : main.cpp ThreadPool.h
	${CC} ${Arg} -c main.cpp

ThreadPool.o : ThreadPool.cpp ThreadPool.h
	${CC} ${Arg} -c ThreadPool.cpp

clean:
	rm *.o

run:
	./main
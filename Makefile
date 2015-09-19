CXX=clang++
FLAGS=-g -ggdb -O0 # -std=c++11 -DUSE_CPP11
all: libcparser.so main
%.o: %.cpp
	${CXX} ${FLAGS} -Wall -pipe -fPIC -c $<
libcparser.so: token.o
	${CXX} ${FLAGS} -Wall -pipe -shared *.o -o libcparser.so
clean:
	rm -f cparser && rm -f *.o
main: main.o token.o
	${CXX} ${FLAGS} main.o token.o -o main
rebuild: clean all


FLAGS=-g -ggdb -O0 # -std=c++11 -DUSE_CPP11
all: libcparser.so main
%.cpp.lib: %.cpp %.h
	${CXX} ${FLAGS} -Wall -pipe -fPIC -c $< -o $<.lib
%.o: %.cpp %.h
	${CXX} ${FLAGS} -Wall -pipe -fPIC -c $<
main.o: main.cpp *.h
	${CXX} ${FLAGS} -Wall -pipe -fPIC -c $<
libcparser.so: parsers.cpp.lib
	${CXX} ${FLAGS} -Wall -pipe -shared *.cpp.lib -o libcparser.so
clean:
	rm -f cparser && rm -f *.o && rm -f *.cpp.lib
main: main.o parsers.o
	${CXX} ${FLAGS} main.o parsers.o -o main
rebuild: clean all


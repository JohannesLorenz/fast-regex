all: cparser
%.o: %.cpp
	g++ -Wall -pipe -fPIC *.cpp -c $<
cparser: token.o
	g++ -Wall -pipe -shared *.o -o cparser
clean:
	rm cparser
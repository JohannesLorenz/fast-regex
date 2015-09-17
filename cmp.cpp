#include <iostream>
#include <cstring>
#include <cstdlib>
int main(int argc, char** argv)

{

	const int max = atoi(argv[1]);
	std::string _str; // "hell no";
	std::cin >> _str;
	const char* str = _str.c_str();
	int x = 0;
	for(int i = 0; i < max; ++i)
	for(int j = 0; j < max; ++j)
	 if(!strncmp(str, "hello", 5))
	  ++x;
	std::cerr << x << std::endl;
	for(int i = 0; i < max; ++i)
	for(int j = 0; j < max; ++j)
	 if(str[0]=='h'&&str[1]=='e'&&str[2]=='l'&&str[3]=='l'&&str[4]=='o')
	  ++x;
	std::cerr << x << std::endl;
	return x%1;
}

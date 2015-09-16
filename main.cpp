#include <string>
#include <iostream>
#include <sstream>
#include "token.h"

void print_err(const char* msg) {
	std::cerr << "Aborting on error caught: " << std::endl
		<< msg << "." << std::endl;
}

int main()
{
	try
	{
		test_regex();

/*		std::string s;
		std::stringstream buffer;
		buffer << std::cin.rdbuf();
		s = buffer.str();

		//, tmp;
		//while(std::cin >> tmp) s+=tmp;
		std::cerr << "input: " << s << std::endl;
		cfile cf(s);*/
	} catch (std::string msg) {
		print_err(msg.c_str());
	} catch (const char* msg) {
		print_err(msg);
	}
	return 0;
}

/*************************************************************************/
/* cparser - a C++03, template based parser                              */
/* Copyright (C) 2015-2015                                               */
/* Johannes Lorenz (jlsf2013 @ sourceforge)                              */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "c_tokens.h"

void print_err(const char* msg) {
	std::cerr << "Aborting on error caught: " << std::endl
		<< msg << "." << std::endl;
}

int main()
{
	int exit_code = EXIT_SUCCESS;
	try
	{
		std::string s;
		std::stringstream buffer;
		buffer << std::cin.rdbuf();
		s = buffer.str();

		//, tmp;
		//while(std::cin >> tmp) s+=tmp;
		std::cerr << "input: " << s << std::endl;
		//cfile cf(s);
	} catch (std::string msg) {
		print_err(msg.c_str());
		exit_code = EXIT_FAILURE;
	} catch (const char* msg) {
		print_err(msg);
		exit_code = EXIT_FAILURE;
	}
	std::cerr << ((exit_code == EXIT_SUCCESS)
			? "Success!"
			: "Failure!")
			<< std::endl;
	return exit_code;
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/main_passwd.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "lib/UserDb.hpp"

/**********************************************************/
using namespace MALTWebviewCpp;

/**********************************************************/
int main(int argc, char **argv)
{
	//check
	if (argc != 2) {
		fprintf(stderr, "USAGE: %s {USER_NAME}\n", argv[0]);
		return EXIT_FAILURE;
	}

	//load
	std::string login = argv[1];
	if (login == "--help" || login == "-h") {
		printf("USAGE: %s {USER_NAME}\n", argv[0]);
		return EXIT_SUCCESS;
	}

	//ask passwd
	std::string pass1 = getpass("Password: ");
	std::string pass2 = getpass("Confirmation: ");

	//check equal
	if (pass1 != pass2) {
		fprintf(stderr, "Invalid password, not matching confirmation !\n");
		return EXIT_FAILURE;
	}
	if (pass1 == "") {
		fprintf(stderr, "Password cannot be invalid !\n");
		return EXIT_FAILURE;
	}

	//set
	UserDb db(true, false);
	db.set(login, pass1);
	db.save();

	//ok
	return EXIT_SUCCESS;
}
#include "File.hpp"

int main(int argc, char * argv[])
{
	MALTJson::JsonFile file(argv[1]);
	return EXIT_SUCCESS;
}

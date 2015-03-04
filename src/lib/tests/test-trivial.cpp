#include <cstdlib>
#include <cstdio>

int main(void)
{
	char * data = new char[16];
	data[0]='\0';
	puts(data);
	delete[]data;
}

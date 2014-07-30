#include <core/AllocTracer.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

#define BUFFER_SIZE 1024

enum Mode
{
	MODE_PRINT,
	MODE_HISTO
};

enum HistoCrit
{
	HISTO_SIZE,
	HISTO_LIFE,
	HISTO_TIME
};

int main(int argc, char ** argv)
{
	Mode mode = MODE_PRINT;
	MATT::Stack * filterStack = NULL;
	size_t filterSize = 0;
	HistoCrit histCrit = HISTO_SIZE;
	
	//params
	if (argc < 2)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.");
		return -1;
	}

	//scan args
	int i = 1;
	const char * fname = NULL;
	std::map<ticks,size_t> histo;
	while (i < argc)
	{
		if (strcmp(argv[i],"--filter") == 0)
		{
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing arguement value for option --filter\n");
				return -1;
			}
			
			if (sscanf(argv[i+1],"stack=%p",&filterStack) == 1) {
			} else if (sscanf(argv[i+1],"size=%lu",&filterSize) == 1) {
			} else {
				fprintf(stderr,"Invalid filter : %s\n",argv[i+1]);
				return -1;
			}
			
			//inc one more
			i++;
		} else if (strcmp(argv[i],"--histo") == 0) {
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing arguement value for option --filter\n");
				return -1;
			}
			
			if (strcmp(argv[i+1],"size") == 0) {
				histCrit = HISTO_SIZE;
			} else {
				fprintf(stderr,"Invlid histogram mode for option --histo : %s\n",argv[i+1]);
				return -1;
			}
			
			mode=MODE_HISTO;
			
			i++;
		} else {
			fname = argv[i];
		}
		
		i++;
	}
	
	//error
	if (fname == NULL)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.\n");
		return -1;
	}
	
	//open the file
	FILE * fp = fopen(argv[1],"r");
	if (fp == NULL)
	{
		perror(argv[1]);
		return -1;
	}
	
	//open json array
	printf("[\n\t");
	
	//read and print as json
	MATT::AllocTracerChunk buffer[BUFFER_SIZE];
	bool first = true;
	while (!feof(fp))
	{
		size_t size = fread(buffer,sizeof(buffer[0]),BUFFER_SIZE,fp);
		for (size_t i = 0 ; i < size ; i++)
		{
			if (filterSize > 0 && filterSize != buffer[i].size)
				continue;
			if (filterStack != NULL & filterStack != buffer[i].allocStack)
				continue;
			
			if (mode == MODE_PRINT)
			{
				if (!first)
					printf(",");
				//printf("{\n\t\tallocStack: %p\n\t\tsize: %lu\n\t\ttimestamp: %llu\n\t\tlifetime: %llu\n\t}",buffer[i].allocStack,buffer[i].size,buffer[i].timestamp,buffer[i].lifetime);
				printf("['%p',%lu,%llu,%llu]\n",buffer[i].allocStack,buffer[i].size,buffer[i].timestamp,buffer[i].lifetime);
				first = false;
			} else if (mode == MODE_HISTO) {
				if (histCrit == HISTO_SIZE)
				{
					//TODO
				}
			}
		}
	}
	
	//closejson array
	printf("]\n");
	
	fclose(fp);
}
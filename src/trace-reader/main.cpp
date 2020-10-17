/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

//std C++
#include <cstdio>
#include <cstdlib>
#include <cstring>
//Reader
#include "TraceReader.hpp"
#include "TraceReaderPrint.hpp"
#include "TraceReaderHisto.hpp"
#include "TraceReaderStackAllocs.hpp"
#include "TraceReaderFragmentation.hpp"
#include "TraceReaderStats.hpp"

/**********************  USING  *********************/
using namespace MALT;

/********************** CONSTS **********************/
static const char * cstHelp = "Usage : %s [--help|-h] [MODE] [FILTER] {FILE.trace}\n\n\
MODE : \n\
   --histo  {size|lifetime|time}\n\
   --frag   {t1} {t2}\n\
   --mem\n\
   --print\n\
   --stats\n\
\n\
FILTER : \n\
   --filter size=SIZE\n\
   --filter stack=STACKID\n\
   --filter at=TIMESTAMP\n";

/*********************  ENUM  ***********************/
enum Mode
{
	MODE_PRINT,
	MODE_HISTO,
	MODE_AT_TIME,
	MODE_FRAGMENTATION,
	MODE_FRAGMENTATION_DETAILS,
	MODE_STATS
};

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	Mode mode = MODE_PRINT;
	HistoCriteria histCrit = HISTO_SIZE;
	ticks t1,t2;
	Filter filter;
	
	//params
	if (argc < 2)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.\n");
		return -1;
	}

	//TODO use a better option parser
	//scan args
	int i = 1;
	const char * fname = NULL;
	while (i < argc)
	{
		if (strcmp(argv[i],"--print") == 0 || strcmp(argv[i],"-p") == 0)
		{
			mode = MODE_PRINT;
		} else if (strcmp(argv[i],"--stats") == 0)
		{
			mode = MODE_STATS;
		} else if (strcmp(argv[i],"--filter") == 0)
		{
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing arguement value for option --filter\n");
				return -1;
			}
			
			if (sscanf(argv[i+1],"stack=%p",(void**)&(filter.filterStack)) == 1) {
			} else if (sscanf(argv[i+1],"size=%zu",&(filter.filterSize)) == 1) {
			} else if (sscanf(argv[i+1],"at=%llu",&(filter.at)) == 1) {
			} else {
				fprintf(stderr,"Invalid filter : %s\n",argv[i+1]);
				return -1;
			}
			
			//inc one more
			i++;
		} else if (strcmp(argv[i],"--frag") == 0) {
			if (argc - 2 == i)
			{
				fprintf(stderr,"Missing argument values for option --frag\n");
				return -1;
			}
			
			mode = MODE_FRAGMENTATION;
			t1 = atol(argv[i+1]);
			t2 = atol(argv[i+2]);
			i+=2;
		} else if (strcmp(argv[i],"--frag-details") == 0) {
			if (argc - 2 == i)
			{
				fprintf(stderr,"Missing argument values for option --frag-details\n");
				return -1;
			}
			
			mode = MODE_FRAGMENTATION_DETAILS;
			t1 = atol(argv[i+1]);
			t2 = atol(argv[i+2]);
			i+=2;

		} else if (strcmp(argv[i],"--histo") == 0) {
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing argument value for option --histo\n");
				return -1;
			}
			
			if (strcmp(argv[i+1],"size") == 0) {
				histCrit = HISTO_SIZE;
			} else if (strcmp(argv[i+1],"lifetime") == 0) {
				histCrit = HISTO_LIFE;
			} else if (strcmp(argv[i+1],"time") == 0) {
				histCrit = HISTO_TIME;
			} else {
				fprintf(stderr,"Invlid histogram mode for option --histo : %s\n",argv[i+1]);
				return -1;
			}
			
			mode=MODE_HISTO;
			
			i++;
		} else if (strcmp(argv[i],"--mem") == 0 || strcmp(argv[i],"-m") == 0) {
			mode=MODE_AT_TIME;
		} else if (strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0) {
			printf(cstHelp,argv[0]);
			return EXIT_SUCCESS;
		} else {
			fname = argv[i];
		}
		
		i++;
	}

	//modes
	TraceReader * reader = NULL;
	switch (mode)
	{
		case MODE_PRINT:
			reader = new TraceReaderPrint(&filter);
			break;
		case MODE_HISTO:
			reader = new TraceReaderHisto(histCrit,&filter);
			break;
		case MODE_AT_TIME:
			reader = new TraceReaderStackAllocs(&filter);
			break;
		case MODE_FRAGMENTATION:
			reader = new TraceReaderFragmentation(t1,t2,false,&filter);
			break;
		case MODE_FRAGMENTATION_DETAILS:
			reader = new TraceReaderFragmentation(t1,t2,true,&filter);
			break;
		case MODE_STATS:
			reader = new TraceReaderStats(&filter);
			break;
		default:
			fprintf(stderr,"Invalid mode : %d\n",mode);
			return EXIT_FAILURE;
	}
	
	//run
	bool status = reader->run(fname);
	
	//clean memory
	delete reader;
	
	//final return
	if (status)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

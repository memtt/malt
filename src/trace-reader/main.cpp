/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

//std C++
#include <cstdio>
#include <cstdlib>
#include <cstring>
//STL C++
#include <map>
//matt internals
#include <core/AllocTracer.hpp>

/********************  MACROS  **********************/
#define BUFFER_SIZE 1024

/********************** CONSTS **********************/
static const char * cstHelp = "Usage : %s [--help|-h] [MODE] [FILTER] {FILE.trace}\n\n\
MODE : \n\
   --histo  {size|lifetime|time}\n\
   --mem\n\
   --print\n\
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
};

/*********************  ENUM  ***********************/
enum HistoCriteria
{
	HISTO_SIZE,
	HISTO_LIFE,
	HISTO_TIME
};

/********************  STRUCT  **********************/
struct Filter
{
	Filter();
	bool accept (MATT::AllocTracerChunk & chunk);
	MATT::Stack * filterStack;
	size_t filterSize;
	ticks at;
};

/*******************  FUNCTION  *********************/
Filter::Filter()
{
	this->at = 0;
	this->filterSize = 0;
	this->filterStack = NULL;
}

/*******************  FUNCTION  *********************/
bool Filter::accept(MATT::AllocTracerChunk& chunk)
{
	return (filterSize == 0 || filterSize == chunk.size) 
	    && (filterStack == NULL || filterStack == chunk.allocStack)
		&& (at == 0 || (chunk.timestamp <= at && chunk.timestamp + chunk.lifetime >= at));
}

/*********************  CLASS  **********************/
class TraceReader
{
	public:
		TraceReader(Filter * filter = NULL);
		virtual ~TraceReader(void);
		bool run(const char * filepath);
		void run(FILE * fp);
	protected:
		virtual void onStart(void) {};
		virtual void onData(MATT::AllocTracerChunk & chunk){};
		virtual void onEnd(void) {};
		Filter * filter;
};

/*******************  FUNCTION  *********************/
TraceReader::TraceReader(Filter * filter)
{
	this->filter = filter;
}

/*******************  FUNCTION  *********************/
TraceReader::~TraceReader(void)
{

}

/*******************  FUNCTION  *********************/
bool TraceReader::run(const char* fname)
{
	//error
	if (fname == NULL)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.\n");
		return false;
	}
	
	//open the file
	FILE * fp = fopen(fname,"r");
	if (fp == NULL)
	{
		perror(fname);
		return false;
	}
	
	//run
	this->run(fp);
	
	//close
	fclose(fp);
	return true;
}

/*******************  FUNCTION  *********************/
void TraceReader::run(FILE* fp)
{
	//error
	if (fp == NULL)
		return;
	
	//handler
	this->onStart();
	
	//loop
	MATT::AllocTracerChunk buffer[BUFFER_SIZE];
	while (!feof(fp))
	{
		size_t size = fread(buffer,sizeof(buffer[0]),BUFFER_SIZE,fp);
		for (size_t i = 0 ; i < size ; i++)
		{
			bool acc = false;

			//check accept
			if (this->filter == NULL)
				acc = true;
			else
				acc = this->filter->accept(buffer[i]);

			//eat if accepted
			if (acc)
				this->onData(buffer[i]);
		}
	}
	
	//handler
	this->onEnd();
}

/*********************  CLASS  **********************/
class TraceReaderPrint : public TraceReader
{
	public:
		TraceReaderPrint(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		bool first;
};

/*******************  FUNCTION  *********************/
TraceReaderPrint::TraceReaderPrint(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onStart(void)
{
	//open json array
	printf("/* [  [ 'STACK_ID', CHUNK_SIZE , CHUNK_TIMESTAMP , CHUNK_LIFETIME ] , ....   ] */\n\n");
	printf("[\n\t");
	this->first = true;
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onEnd(void)
{
	//closejson array
	printf("\n]\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onData(MATT::AllocTracerChunk& chunk)
{
	if (!first)
		printf(",\n\t");
	//printf("{\n\t\tallocStack: %p\n\t\tsize: %lu\n\t\ttimestamp: %llu\n\t\tlifetime: %llu\n\t}",buffer[i].allocStack,buffer[i].size,buffer[i].timestamp,buffer[i].lifetime);
	printf("['%p',%lu,%llu,%llu]",chunk.allocStack,chunk.size,chunk.timestamp,chunk.lifetime);
	first = false;
}

/*********************  CLASS  **********************/
class TraceReaderHisto : public TraceReader
{
	public:
		TraceReaderHisto(HistoCriteria histCrit,Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		HistoCriteria histCrit;
		std::map<ticks,size_t> histo;
};

/*******************  FUNCTION  *********************/
TraceReaderHisto::TraceReaderHisto(HistoCriteria histCrit, Filter * filter): TraceReader(filter)
{
	this->histCrit = histCrit;
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onStart(void)
{
	this->histo.clear();
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onEnd(void)
{
	//open json array
	printf("{\n");
	
	//print
	for (std::map<ticks,size_t>::const_iterator it = histo.begin() ; it != histo.end() ; ++it)
		printf("	%llu:%lu,\n",it->first,it->second);
	
	//closejson array
	printf("}\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onData(MATT::AllocTracerChunk& chunk)
{
	ticks indice;

	switch(histCrit)
	{
		case HISTO_SIZE:
			indice = chunk.size;
			break;
		case HISTO_LIFE:
			indice = chunk.lifetime;
			break;
		case HISTO_TIME:
			indice = chunk.timestamp;
			break;
	}
	
	std::map<ticks,size_t>::iterator it = histo.find(indice);
	if (it == histo.end())
		histo[indice] = 1;
	else
		it->second++;
}

/********************  STRUCT  **********************/
struct AtTimeInfo
{
	AtTimeInfo(void) {this->count = 0;this->size = 0;};
	size_t count;
	size_t size;
};

/*********************  CLASS  **********************/
class TraceReaderStackAllocs : public TraceReader
{
	public:
		TraceReaderStackAllocs(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		std::map<const MATT::Stack *,AtTimeInfo> map;
};

/*******************  FUNCTION  *********************/
TraceReaderStackAllocs::TraceReaderStackAllocs(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onStart(void)
{
	this->map.clear();
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onEnd(void)
{
	bool first = true;
	printf("[");
	for (std::map<const MATT::Stack *,AtTimeInfo>::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		if (!first)
		{
			first = false;
			printf(",");
		}
		printf("\n\t%p: [%lu,%lu]",it->first,it->second.count,it->second.size);
	}
	printf("\n]\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onData(MATT::AllocTracerChunk& chunk)
{
	AtTimeInfo & info = map[chunk.allocStack];
	info.count++;
	info.size+=chunk.size;
	//printf("cur = %llu , %p => OK\n",chunk.timestamp,chunk.allocStack);
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	Mode mode = MODE_PRINT;
	HistoCriteria histCrit = HISTO_SIZE;
	Filter filter;
	
	//params
	if (argc < 2)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.");
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
		} else if (strcmp(argv[i],"--filter") == 0)
		{
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing arguement value for option --filter\n");
				return -1;
			}
			
			if (sscanf(argv[i+1],"stack=%p",&(filter.filterStack)) == 1) {
			} else if (sscanf(argv[i+1],"size=%lu",&(filter.filterSize)) == 1) {
			} else if (sscanf(argv[i+1],"at=%llu",&(filter.at)) == 1) {
			} else {
				fprintf(stderr,"Invalid filter : %s\n",argv[i+1]);
				return -1;
			}
			
			//inc one more
			i++;
		} else if (strcmp(argv[i],"--histo") == 0) {
			if (argc - 1 == i)
			{
				fprintf(stderr,"Missing arguement value for option --histo\n");
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

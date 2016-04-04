/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//internals
#include "JsonReader.hpp"
//from core
#include "common/Debug.hpp"
//std
#include <cstdio>
#include <cstring>
//unix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
JsonReader::~JsonReader ( void )
{

}

/*******************  FUNCTION  *********************/
JsonReader::JsonReader ( void )
{
	this->raw = NULL;
}

/*******************  FUNCTION  *********************/
JsonNode& JsonReader::getRoot ( void )
{
	return root;
}

/*******************  FUNCTION  *********************/
void JsonReader::loadString ( const char* value )
{
	this->raw = strdup(value);
	this->parse();
}

/*******************  FUNCTION  *********************/
void JsonReader::load ( const std::string& filename )
{
	//check
	assume(filename.empty() == false,"Invalid empty filename");
	assume(this->raw == NULL,"You try to reload an already loaded content.");
	
	//open
	FILE * fp  = fopen(filename.c_str(),"r");
	assumeArg(fp != NULL,"Fail to open file %1 : %2").arg(filename).argStrErrno().end();
	
	//get size
	int fd = fileno(fp);
	size_t size = getFileSize(fd);
	assumeArg(size > 0,"Get invalid file size : %1 : %2").arg(filename).arg(size).end();
	
	//allocate memory (size + 1 for last \0)
	this->raw = new char[size+1];
	
	//load it into memory
	size_t readed = fread(this->raw,1,size,fp);
	assumeArg(readed == size,"Failed to read the whole file into memory, get %1 bytes instead of %2 bytes")
		.arg(readed)
		.arg(size)
		.end();
		
	//mark last \0
	this->raw[readed] = '\0';
	
	//close
	fclose(fp);
	
	//now parse
	this->parse();
}

/*******************  FUNCTION  *********************/
void JsonReader::parse ( void )
{
	//checkes
	assert(this->raw != NULL);
	
	//ensure init of stack alloc
	MALTV2::doNoFreeAllocatorInit();
	
	//forward
	this->root.loadFromData(this->raw,true);
}

/*******************  FUNCTION  *********************/
void JsonReader::ungzip ( void )
{
	MALT_FATAL("Not supported");
}

/*******************  FUNCTION  *********************/
size_t JsonReader::getFileSize ( int fd )
{
    struct stat stat_buf;
    int rc = fstat(fd, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

}

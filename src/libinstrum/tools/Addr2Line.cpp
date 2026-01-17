/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/Addr2Line.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <common/Debug.hpp>
#include <common/Options.hpp>
#include <portability/OS.hpp>
#include "Addr2Line.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
Addr2Line::Addr2Line(StringIdDictionnary & dict, const std::string & elfFile, size_t aslrOffset, size_t bucketSize)
	:dict(dict)
{
	//checks
	assert(elfFile.empty() == false);

	//set
	this->elfFile = elfFile;
	this->elfFileId = this->dict.getId(String(elfFile.c_str()));
	this->aslrOffset = aslrOffset;
	this->bucketSize = bucketSize;

	//check if huge (> 50 MB)
	if (OS::getFileSize(elfFile) > gblOptions->stackAddr2lineHuge) {
		this->isHugeElfFile = true;
		this->bucketSize = SIZE_MAX;
	}
}

/**********************************************************/
bool Addr2Line::isFull(void) const
{
	return this->tasks.size() >= this->bucketSize;
}

/**********************************************************/
bool Addr2Line::isHugeElf(void) const
{
	return this->isHugeElfFile;
}

/**********************************************************/
bool Addr2Line::addTask(const LangAddress & address, CallSite * callSite)
{
	//checks
	assert(address.getDomain() == DOMAIN_C);
	assert(address.isNULL() == false);
	assert(callSite != nullptr);

	//check
	if (this->tasks.size() >= this->bucketSize)
		return false;

	//append
	tasks.emplace_back();
	Addr2LineTask & task = this->tasks.back();
	task.address = address;
	task.callSite = callSite;

	//ok
	return true;
}

/**********************************************************/
bool Addr2Line::run(void)
{
	//trivial
	if (this->tasks.empty())
		return true;

	//file buffer
	std::string fileBuffer;
	int fd = 0;
	if (this->isHugeElfFile) {
		char templ[] = "/tmp/malt-addr2line-XXXXXX";
		fd = mkstemp(templ);
		char * buffer = (char*)MALT_MALLOC(4096);
		char * path = (char*)MALT_MALLOC(4096);
		snprintf(path, 4096, "/proc/self/fd/%d", fd);
		ssize_t status = readlink(path, buffer, sizeof(buffer));
		assumeArg(status > 0, "Fail to get symlink translation : %1").arg(path).end();
		fileBuffer = buffer;
		MALT_FREE(path);
		MALT_FREE(buffer);
	}

	//build command
	std::string command = this->buildCommandLine(fileBuffer);
	//MALT_DEBUG_ARG("addr2line", "Command : %1").arg(command).end();
	assert(command.size() < 4096);

	//debug
	if (gblOptions != NULL && gblOptions->outputVerbosity >= MALT_VERBOSITY_VERBOSE)
		fprintf(stderr, "MALT: %s\n",command.c_str());

	//run it
	FILE * fp = popen(command.c_str(),"r");
	if (fp == NULL)
	{
		MALT_ERROR_ARG("Fail to use addr2line on %1 to load symbols : %2.").arg(elfFile).argStrErrno().end();
		return false;
	}

	//parse output
	for (auto & task : tasks) {
		const bool status = this->loadEntry(*task.callSite, fp);
		if (status != 0) assert(false);
	}

	//close
	int res = pclose(fp);

	//remove temp file
	if (fileBuffer.empty() == false) {
		unlink(fileBuffer.c_str());
		close(fd);
	}

	//check result
	if (res != 0)
	{
		MALT_ERROR_ARG("Get error while using addr2line on %1 to load symbols : %2.").arg(elfFile).argStrErrno().end();
		return false;
	}

	//ok
	return true;
}

/**********************************************************/
std::string Addr2Line::buildCommandLine(const std::string & fileBuffer) const
{
	//build
	std::stringstream command;

	//base
	command << "addr2line -C -f -e " << elfFile;

	//pointer to the stream
	std::ostream * addresses = &command;
	std::ofstream file;
	if (fileBuffer.empty() == false) {
		file.open(fileBuffer);
		assumeArg(file.good(), "Fail to open %1 : %2").arg(fileBuffer).argStrErrno().end();
		addresses = &file;
		command << " @" << fileBuffer;
	}
	command << " ";

	//add the addresses
	for (auto task : tasks) {
		//check
		assert(task.address.getDomain() == DOMAIN_C);

		//append
		*addresses << (void*)((((size_t)task.address.getAddress()) - this->aslrOffset)) << ' ';
	}

	//silent
	if (gblOptions != NULL && gblOptions->outputVerbosity <= MALT_VERBOSITY_DEFAULT)
		command << " 2>/dev/null";

	//ok
	return command.str();
}

/**********************************************************/
bool Addr2Line::loadEntry(CallSite & callSite, FILE * fp)
{
	//check
	assert(fp != nullptr);

	//read the two lines
	char * function = fgets(bufferFunc,sizeof(bufferFunc)-1,fp);
	char * file = fgets(bufferFile,sizeof(bufferFile)-1,fp);

	//check
	if (function != bufferFunc || file != bufferFile)
		return false;

	//check
	assume(strlen(function) < sizeof(bufferFunc) - 2, "Overflow buffer size !");
	assume(strlen(file) < sizeof(bufferFile) - 2, "Overflow buffer size !");

	//debug
	//MALT_DEBUG_ARG("addr2line", "function=%1, source=%2").arg(bufferFunc).arg(bufferFile).end();

	//check end of line and remove it
	int endLine = strlen(bufferFunc);
	assumeArg(bufferFunc[endLine-1] == '\n',"Missing \\n at end of line for the function or symbol name read from addr2line : %1.").arg(bufferFunc).end();
	bufferFunc[endLine-1] = '\0';

	//search ':' separator at end of "file:line" string
	char * sep = strrchr(bufferFile,':');
	if (sep == NULL)
	{
		MALT_WARNING_ARG("Fail to split source location on ':' : %1").arg(bufferFile).end();
	} else {
		*sep='\0';

		//extract line
		callSite.line = atoi(sep+1);

		//get filename and function name address
		callSite.binary = elfFileId;

		//get filename and function name address
		callSite.file = this->dict.getId(bufferFile);

		assert(callSite.file != -1);
		//if (strcmp(bufferFunc,"??") == 0)
		//      lst[i]->function = -1;
		//else
		callSite.function = this->dict.getId(bufferFunc);
	}

	//ok
	return true;
}

}

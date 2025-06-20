/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/ELFReader_fake.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "ELFReader.hpp"
//standard
#include <cstdio>
#include <cassert>
//internals
#include <common/Debug.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
ElfReader::ElfReader(const std::string& file)
{
	this->elf = NULL;
	this->fp = NULL;
}

/**********************************************************/
ElfReader::~ElfReader(void)
{
}

/**********************************************************/
void ElfReader::loadGlobalVariables(ElfGlobalVariableVector& variables)
{
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const ElfGlobalVariable& value)
{
}

/**********************************************************/
bool ElfReader::hasLibElf(void)
{
	return false;
}

const ElfGlobalVariable & ElfReader::getVarByName(const ElfGlobalVariableVector & variables, const std::string & name) const
{
	static ElfGlobalVariable var;
	return var;
}

/**********************************************************/
void * ElfReader::getInMemAddr(const LinuxProcMapReader & procMap, const ElfGlobalVariable & variable) const
{
	return nullptr;
}

/**********************************************************/
size_t ElfReader::getPhysSize(const LinuxProcMapReader & procMapReader, ProcPageMapReader & pagePageMapReader, const ElfGlobalVariable & variable) const
{
	return 0;
}

}

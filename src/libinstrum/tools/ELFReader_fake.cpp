/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/ELFReader_fake.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
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

}

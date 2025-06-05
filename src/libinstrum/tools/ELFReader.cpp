/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/ELFReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
***********************************************************/

/**********************************************************/
#include "ELFReader.hpp"
//standard
#include <cstdio>
#include <cassert>
//internals
#include <common/Debug.hpp>
//from libelf (non standard)
#include <libelf.h>
#include "portability/Compiler.hpp"
#include "portability/OS.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#if defined(__x86_64__) || defined(__aarch64__)
	#define ElfArch_Ehdr Elf64_Ehdr
	#define elfarch_getehdr(x) elf64_getehdr(x)
	#define ElfArch_Shdr Elf64_Shdr
	#define elfarch_getshdr(x) elf64_getshdr(x)
	#define ElfArch_Sym Elf64_Sym
	#define ELFARCH_ST_BIND(x) ELF64_ST_BIND(x)
	#define ELFARCH_ST_TYPE(x) ELF64_ST_TYPE(x)
#elif defined(__i386__) || defined(__aarch__)
	#define ElfArch_Ehdr Elf32_Ehdr
	#define elfarch_getehdr(x) elf32_getehdr(x)
	#define ElfArch_Shdr Elf32_Shdr
	#define elfarch_getshdr(x) elf32_getshdr(x)
	#define ElfArch_Sym Elf32_Sym
	#define ELFARCH_ST_BIND(x) ELF32_ST_BIND(x)
	#define ELFARCH_ST_TYPE(x) ELF32_ST_TYPE(x)
#else
	#warning "Not tested archicture, using 64bit mode"
	#define ElfArch_Ehdr Elf64_Ehdr
	#define elfarch_getehdr(x) elf64_getehdr(x)
	#define ElfArch_Shdr Elf64_Shdr
	#define elfarch_getshdr(x) elf64_getshdr(x)
	#define ElfArch_Sym Elf64_Sym
	#define ELFARCH_ST_BIND(x) ELF64_ST_BIND(x)
	#define ELFARCH_ST_TYPE(x) ELF64_ST_TYPE(x)
#endif

/**********************************************************/
/**
 * Constructor of the elf reader. It will immediatly open
 * the given file.
 * @param file ELF file to open.
**/
ElfReader::ElfReader(const std::string& file)
{
	//defaults
	this->elf = NULL;
	this->fp = NULL;

	//check init
	this->libelfInit();
	
	//open
	this->openFile(file);
}

/**********************************************************/
/**
 * Destructor of elf reader to automatically close libelf and file.
**/
ElfReader::~ElfReader(void)
{
	this->close();
}

/**********************************************************/
/**
 * Close the libelf handler and related file handler.
**/
void ElfReader::close(void)
{
	//fp
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	
	//elf
	if (this->elf != NULL)
	{
		elf_end(elf);
		elf = NULL;
	}
}

/**********************************************************/
/**
 * Open file and link with libelf to be ready for elf analysis.
 * @param file Define the file to open.
**/
void ElfReader::openFile(const std::string& file)
{
	//vars
	Elf_Kind kind;
	
	//open
	fp = fopen(file.c_str(),"r");
	
	assumeArg(fp != NULL,"Failed to open file %1 : %2")
		.arg(file)
		.argStrErrno()
		.end();

	//start elf
	this->elf = elf_begin(fileno(fp),ELF_C_READ,NULL);
	if (this->elf == NULL)
	{
		//caution, elf_errno reset the error gbl var to 0, not call it twice.
		const int errId = elf_errno();
		MALT_WARNING_ARG("Failed to open ELF file : %1. Error %2 : %3")
			.arg(file)
			.arg(errId)
			.arg(elf_errmsg(errId))
			.end();
		goto err_elf_begin;
	}

	//check kind of ELF
	kind = elf_kind(elf);
	if (kind != ELF_K_ELF)
	{
		MALT_WARNING_ARG("Invalid kind of elf file, expect ELF (%1), get %2")
			.arg(ELF_K_ELF)
			.arg(kind)
			.end();
		goto err_elf_kind;
	}

	//track
	this->binaryFile = file;

	//ok valid
	return;

	//error management
	err_elf_kind:
		elf_end(elf);
		this->elf = NULL;
	err_elf_begin:
		fclose(fp);
		fp = NULL;
}

/**********************************************************/
/**
 * Libelf need some initialization stuff to work so ensure it's done
 * at least once.
**/
void ElfReader::libelfInit(void)
{
	//check to do it only once
	static bool initDone = false;

	//check already done
	if (initDone)
		return;
	
	//check version (don't know why, but do not work without this)
	elf_version(EV_CURRENT);
	
	//mark as done
	initDone = true;
}

/**********************************************************/
/**
 * Search the section corresponding to the given type.
 * @param type Type of section to return.
 * @return Pointer to the section (freed by libelf) of NULL if not found.
**/
Elf_Scn* ElfReader::getSectionByType(size_t type)
{
	//errors
	assert(elf != NULL);
	
	//extract header
	ElfArch_Ehdr * header = elfarch_getehdr(elf);
	assert(header != NULL);
	
	//get sec number
	int secCount = header->e_shnum;
	assert(secCount > 0);

	//loop on sections
	for (int s = 0 ; s < secCount ; s++)
	{
		//get section
		Elf_Scn * sec = elf_getscn(elf,s);
		assert(sec != NULL);
		ElfArch_Shdr * secHeader = elfarch_getshdr(sec);
		assert(secHeader != NULL);
		
		//check type
		if (secHeader->sh_type == type)
			return sec;
	}
	
	//not found
	return NULL;
	
}

/**********************************************************/
void ElfReader::loadGlobalVariables(ElfGlobalVariableVector& variables)
{
	//check errors
	if (this->elf == NULL)
	{
		MALT_WARNING("Caution, get NULL elf structure, ignore it and skip global variable extraction.");
		return;
	}
	
	//get section
	Elf_Scn * sec = getSectionByType(SHT_SYMTAB);
	if (sec == NULL)
		sec = getSectionByType(SHT_DYNSYM);
	assert(sec != NULL);
	
	//check header
	ElfArch_Shdr * secHeader = elfarch_getshdr(sec);
	assert(secHeader != NULL);
	assert(secHeader->sh_type == SHT_SYMTAB || secHeader->sh_type == SHT_DYNSYM);
	
	//check entry type
	assert(secHeader->sh_entsize == sizeof(ElfArch_Sym));
	assert(secHeader->sh_size % secHeader->sh_entsize == 0);
			
	//load
	Elf_Data * data = elf_getdata(sec,NULL);
	assert(data != NULL);
	ElfArch_Sym * table = (ElfArch_Sym*)data->d_buf;
	assert((Elf_Data*)table != NULL);
			
	//count symbols
	int symCount = secHeader->sh_size / secHeader->sh_entsize;
			
	//extract string table by following 'link'
	int link = secHeader->sh_link;
	assert(link > 0);
	ElfStringTable strings = getStringTable(link);
	assert(strings.data != NULL && strings.size > 0);

	//init
	std::map<size_t, size_t> offsetMap;
	for (int i = 0 ; i < symCount ; i++)
		offsetMap[table[i].st_shndx] = secHeader->sh_offset;

	//loop on symbols
	//size_t offset = secHeader->sh_offset;
	for (int i = 0 ; i < symCount ; i++)
	{
		if (table[i].st_name != 0)
		{
			//extract types
			int bind = ELFARCH_ST_BIND(table[i].st_info);
			int type = ELFARCH_ST_TYPE(table[i].st_info);

			//check type, only extract global and tls
			if ((type == STT_OBJECT || type == STT_TLS) && (bind == STB_LOCAL || bind == STB_GLOBAL) && table[i].st_size > 0)
			{
				//get section
				Elf_Scn * vSec = elf_getscn(elf,table[i].st_shndx);
				assert(sec != NULL);
				ElfArch_Shdr * vSecHeader = elfarch_getshdr(vSec);
				assert(secHeader != NULL);

				//setup var
				ElfGlobalVariable var;
				if (table[i].st_name > 0 && table[i].st_name < strings.size)
					var.symbol = strings.data + table[i].st_name;
				var.size = table[i].st_size;
				var.tls = (type == STT_TLS);
				var.line = -1;
				var.offset = table[i].st_value;
				var.secOffset = vSecHeader->sh_offset;
				var.binaryFile = this->binaryFile;

				//fix name
				//get short name to cut on recent GCC (eg. _ZSt4cout@GLIBCXX_3.4)
				std::string shortName = var.symbol;
				int pos = shortName.find("@");
				if (pos != std::string::npos)
					shortName = shortName.substr(0, pos);
				
				//demangle namespace
				var.name = Compiler::demangleCppNames(shortName);
				
				//push
				variables.push_back(var);
				//printf("symbol = %d = %s -> %zu\n",table[i].st_name,strings.get(table[i].st_name),table[i].st_size);
			}
		}

		//inc
		offsetMap[table[i].st_shndx] += table[i].st_size;
	}
}

/**********************************************************/
ElfStringTable ElfReader::getStringTable(int secId)
{
	//res
	ElfStringTable res = {NULL,0};
	
	//get section
	Elf_Scn * sec = elf_getscn(elf,secId);
	assert(sec != NULL);
	ElfArch_Shdr * secHeader = elfarch_getshdr(sec);
	assert(secHeader != NULL);
	
	//check kind
	assumeArg(secHeader->sh_type == SHT_STRTAB,"Invalid section type, expect STRTAB (%d), get %d !")
		.arg(SHT_STRTAB)
		.arg(secHeader->sh_type)
		.end();
		
	//get data
	Elf_Data * data = elf_getdata(sec,NULL);
	assert(data != NULL);
		
	//convert to internal struct
	res.data = (const char *)data->d_buf;
	assert(res.data != NULL);
	res.size = secHeader->sh_size;
	
	//return
	return res;
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const ElfGlobalVariable& value)
{
	json.openStruct();
		json.printField("symbol",value.name);
		json.printField("name",value.name);
		json.printField("size",value.size);
		json.printField("offset",value.offset);
		json.printField("secOffset",value.secOffset);
		json.printField("tls",value.tls);
		json.printField("usedSize",value.usedSize);
		if (value.line != -1 && !value.sourceFile.empty())
		{
			json.printField("line",value.line);
			json.printField("sourceFile",value.sourceFile);
		}
		json.printField("binaryFile",value.binaryFile);
	json.closeStruct();
}

/**********************************************************/
bool ElfReader::hasLibElf(void)
{
	return true;
}

/**********************************************************/
const ElfGlobalVariable & ElfReader::getVarByName(const ElfGlobalVariableVector & variables, const std::string & name) const
{
	for (const auto & it : variables) {
		if (it.name == name)
			return it;
	}
	MALT_FATAL_ARG("Fail to find global variable {1}").arg(name).end();
	return *(ElfGlobalVariable*)nullptr;
}

/**********************************************************/
void * ElfReader::getInMemAddr(const LinuxProcMapReader & procMap, const ElfGlobalVariable & variable) const
{
	const LinuxProcMapEntry * entry = procMap.getEntryByOffset(variable.binaryFile, variable.offset + variable.secOffset);
	assert(entry != nullptr);

	//TODO fix
	if (entry->aslrOffset == -1UL || entry->aslrOffset == 0UL) {
		((LinuxProcMapEntry * )entry)->aslrOffset = OS::getASLROffset((void*)((size_t)entry->lower + variable.offset));
	}

	return (void*)(entry->aslrOffset + variable.offset);
}

/**********************************************************/
size_t ElfReader::getPhysSize(const LinuxProcMapReader & procMapReader, ProcPageMapReader & pagePageMapReader, const ElfGlobalVariable & variable) const
{
	if (variable.tls) {
		return 0;
	} else {
		void * addr = this->getInMemAddr(procMapReader, variable);
		return pagePageMapReader.getPhysicalSize(addr, variable.size);
	}
}

}

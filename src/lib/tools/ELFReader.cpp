/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "ELFReader.hpp"
//standard
#include <cstdio>
#include <cassert>
//internals
#include <common/Debug.hpp>
//from libelf (non standard)
#include <libelf.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  MACROS  **********************/
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

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
/**
 * Destructor of elf reader to automatically close libelf and file.
**/
ElfReader::~ElfReader(void)
{
	this->close();
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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
		MALT_WARNING_ARG("Failed to open ELF file : %1. Error %2 : %3")
			.arg(file)
			.arg(elf_errno())
			.arg(elf_errmsg(elf_errno()))
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

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

	//loop on symbols
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
				//setup var
				ElfGlobalVariable var;
				if (table[i].st_name > 0 && table[i].st_name < strings.size)
					var.name = strings.data + table[i].st_name;
				var.size = table[i].st_size;
				var.tls = (type == STT_TLS);
				var.line = -1;
				
				//push
				variables.push_back(var);
				//printf("symbol = %d = %s -> %zu\n",table[i].st_name,strings.get(table[i].st_name),table[i].st_size);
			}
		}
	}
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const ElfGlobalVariable& value)
{
	json.openStruct();
		json.printField("name",value.name);
		json.printField("size",value.size);
		//json.printField("offset",value.offset);
		json.printField("tls",value.tls);
		if (value.line != -1 && !value.file.empty())
		{
			json.printField("line",value.line);
			json.printField("file",value.file);
		}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
bool ElfReader::hasLibElf(void)
{
	return true;
}

}

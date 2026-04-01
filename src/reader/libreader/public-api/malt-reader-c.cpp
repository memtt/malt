/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "libinstrum/portability/Visibility.hpp"
#include "malt-reader-internal.h"
#include "malt-reader-c.h"

/**********************************************************/
//traces
DLL_PUBLIC size_t malt_reader_c_get_trace_count(struct malt_reader_t * reader)
{
	//if nullptr
	if (reader == nullptr)
		return 0;

	//access to the info
	return reader->profile->getTrace().size();
}

/**********************************************************/
DLL_PUBLIC const malt_trace_event_t * malt_reader_c_get_trace_event(struct malt_reader_t * reader, size_t id)
{
	//if null
	if (reader == nullptr)
		return nullptr;

	//check in range
	const Trace & trace = reader->profile->getTrace();
	if (id >= trace.size())
		return nullptr;

	//get it
	return &trace.at(id);
}

/**********************************************************/
//stack
DLL_PUBLIC size_t malt_reader_c_get_stack_size(struct malt_reader_t * reader, const malt_stack_t * stack)
{
	//trivial case
	if (reader == nullptr || stack == nullptr)
		return 0;

	//cast
	const MALTFormat::Stack * castedStack = (const MALTFormat::Stack *)stack;

	//check size
	return castedStack->size();
}

/**********************************************************/
static const char * malt_reader_c_std_string_to_char_ptr(const std::string * value)
{
	if (value == nullptr)
		return nullptr;
	else
		return value->c_str();
}

/**********************************************************/
DLL_PUBLIC size_t malt_reader_c_load_stack_infos(struct malt_reader_t * reader, const malt_stack_t * stack, malt_stack_infos_t * stack_infos, size_t stack_info_length)
{
	//trivial
	if (reader == nullptr || stack == nullptr || stack_infos == nullptr || stack_info_length == 0)
		return 0;

	//cast
	const MALTFormat::Stack * castedStack = (const MALTFormat::Stack *)stack;
	const Extractor & extractor = reader->profile->getExtractor();

	//vars
	size_t cursor = 0;
	for (const auto & langAddr : *castedStack) {
		//check if stop
		if (cursor >= stack_info_length)
			return cursor;

		//extract infos
		const InstructionInfosStrRef & infos = extractor.getAddrTranslation(langAddr);

		//copy
		stack_infos[cursor].uid = (size_t)&infos;
		stack_infos[cursor].lang_addr.lang = malt_profile_lang_t(infos.origin.lang);
		stack_infos[cursor].lang_addr.address = infos.origin.address;
		stack_infos[cursor].file = malt_reader_c_std_string_to_char_ptr(infos.file);
		stack_infos[cursor].binary = malt_reader_c_std_string_to_char_ptr(infos.binary);
		stack_infos[cursor].function = malt_reader_c_std_string_to_char_ptr(infos.function);
		stack_infos[cursor].functionShort = malt_reader_c_std_string_to_char_ptr(infos.functionShort);
		stack_infos[cursor].line = infos.line;
		stack_infos[cursor].offset = infos.offset;

		//incr
		cursor++;
	}

	return cursor;
}

/**********************************************************/
DLL_PUBLIC bool malt_reader_c_is_source_file(struct malt_reader_t* reader, const char * path)
{
	return reader->profile->isSourceFile(path);
}

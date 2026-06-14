/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader-c.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "trace/ProcessMemModel.hpp"
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
DLL_PUBLIC bool malt_reader_c_load_stack_infos(struct malt_reader_t * reader, const malt_stack_t * stack, malt_stack_infos_t * stack_infos, size_t id)
{
	//nothing to do
	if (stack_infos == nullptr)
		return false;

	//reset the content
	stack_infos->uid = 0;
	stack_infos->lang_addr.lang = MALT_PROFILE_LANG_C;
	stack_infos->lang_addr.address = nullptr;
	stack_infos->file = nullptr;
	stack_infos->binary = nullptr;
	stack_infos->function = nullptr;
	stack_infos->functionShort = nullptr;
	stack_infos->line = -1;
	stack_infos->offset = 0;

	//trivial
	if (reader == nullptr || stack == nullptr)
		return false;

	//cast
	const MALTFormat::Stack * castedStack = (const MALTFormat::Stack *)stack;
	const Extractor & extractor = reader->profile->getExtractor();

	//not ok
	if (id >= castedStack->size())
		return false;

	//reverse to get root first
	id = castedStack->size() - id - 1;

	//vars
	const auto & langAddr = castedStack->at(id);
	
	//extract infos
	const InstructionInfosStrRef & infos = extractor.getAddrTranslation(langAddr);

	//copy
	stack_infos->uid = (size_t)&infos;
	stack_infos->lang_addr.lang = malt_profile_lang_t(infos.origin.lang);
	stack_infos->lang_addr.address = infos.origin.address;
	stack_infos->file = malt_reader_c_std_string_to_char_ptr(infos.file);
	stack_infos->binary = malt_reader_c_std_string_to_char_ptr(infos.binary);
	stack_infos->function = malt_reader_c_std_string_to_char_ptr(infos.function);
	stack_infos->functionShort = malt_reader_c_std_string_to_char_ptr(infos.functionShort);
	stack_infos->line = infos.line;
	stack_infos->offset = infos.offset;

	return true;
}

/**********************************************************/
DLL_PUBLIC bool malt_reader_c_is_source_file(struct malt_reader_t* reader, const char * path)
{
	return reader->profile->isSourceFile(path);
}

/**********************************************************/
DLL_PUBLIC malt_process_mem_model_t * malt_reader_c_process_mem_model_create(void)
{
	return (malt_process_mem_model_t *)(new ProcessMemModel());
}

/**********************************************************/
DLL_PUBLIC void malt_reader_c_process_mem_model_play_event(malt_process_mem_model_t * model, const malt_trace_event_t * event)
{
	//nothing to do
	if (model == nullptr || event == nullptr)
		return;

	//cast
	ProcessMemModel * castedModel = (ProcessMemModel*)model;

	//send event
	castedModel->playNextEvent(*(MALT::AllocTracerEvent*)event);
}

/**********************************************************/
DLL_PUBLIC malt_process_mem_counter_t malt_reader_c_process_mem_model_get(malt_process_mem_model_t * model, malt_process_mem_domain_t domain)
{
	//vars
	malt_process_mem_counter_t res = {0,0};

	//nothing to do
	if (model == nullptr)
		return res;

	//cast
	ProcessMemModel * castedModel = (ProcessMemModel*)model;

	//return
	const ProcessMemDomainCounter counter = castedModel->getCounters(ProcessMemDomain(domain));

	//copy
	res.alive_chunks = counter.aliveChunks;
	res.virtual_memory = counter.usedVirtualMemory;

	//return 
	return res;
}

/**********************************************************/
DLL_PUBLIC void malt_reader_c_process_mem_model_delete(malt_process_mem_model_t * model)
{
	//nothing to do
	if (model == nullptr)
		return;

	//cast
	ProcessMemModel * castedModel = (ProcessMemModel*)model;
	
	//delete
	delete castedModel;
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Options.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//std
#include <cstdio>
#include <cassert>
#include <set>
//internals
#include "Debug.hpp"
#include "Options.hpp"
#include <json/JsonState.h>

/**********************************************************/
namespace MALT 
{

/**********************************************************/
Options * gblOptions = NULL;
/**
 * To be used in LocalAllocStackProfiler to cut MALT internal function calls
 * when backtracing and stop exactly at malloc/free level.
 * Made as an option as I already had serveral time issues depending on compiler
 * inlining operations so in case it append to someone he can fix without
 * recompiling.
**/
static const int cstDefaultStackSkip = 4;

/**********************************************************/
/**
 * To convert MALT verbosity level.
**/
static const char * cstVerbosityLevels[MALT_VERBOSITY_COUNT] = {
	"silent",
	"default",
	"verbose"
};

/**********************************************************/
/**
 * Constructor to setup the default values for each options
**/
Options::Options(void)
{
}

/**********************************************************/
/**
 * Manage operator == to help validation in unit test suite.
**/
bool Options::operator==(const Options& value) const
{
	//stack
	if (stackProfileEnabled != value.stackProfileEnabled) return false;
	if (stackResolve != value.stackResolve) return false;
	if (stackMode != value.stackMode) return false;
	if (stackLibunwind != value.stackLibunwind) return false;
	if (stackSkip != value.stackSkip) return false;
	if (stackAddr2lineBucket != value.stackAddr2lineBucket) return false;
	if (stackAddr2lineThreads != value.stackAddr2lineThreads) return false;
	if (stackAddr2lineHuge != value.stackAddr2lineHuge) return false;
	if (stackSampling != value.stackSampling) return false;
	if (stackSamplingBw != value.stackSamplingBw) return false;
	//python
	if (pythonInstru != value.pythonInstru) return false;
	if (pythonStack != value.pythonStack) return false;
	if (pythonMix != value.pythonMix) return false;
	if (pythonObj != value.pythonObj) return false;
	if (pythonMem != value.pythonMem) return false;
	if (pythonRaw != value.pythonRaw) return false;
	if (pythonHideImports != value.pythonHideImports) return false;
	if (pythonMode != value.pythonMode) return false;
	//c
	if (cMalloc != value.cMalloc) return false;
	if (cMmap != value.cMmap) return false;
	//time
	if (this->timeProfileEnabled != value.timeProfileEnabled) return false;
	if (this->timeProfilePoints != value.timeProfilePoints) return false;
	if (this->timeProfileLinear != value.timeProfileLinear) return false;
	//output
	if (this->outputName != value.outputName) return false;
	if (this->outputIndent != value.outputIndent) return false;
	if (this->outputJson != value.outputJson) return false;
	if (this->outputLua != value.outputLua) return false;
	if (this->outputCallgrind != value.outputCallgrind) return false;
	if (this->outputDumpConfig != value.outputDumpConfig) return false;
	if (this->outputVerbosity != value.outputVerbosity)  return false;
	if (this->outputStackTree != value.outputStackTree) return false;
	if (this->outputLoopSuppress != value.outputLoopSuppress) return false;
	//max stack
	if (this->maxStackEnabled != value.maxStackEnabled) return false;
	//maps
	if (this->distrAllocSize != value.distrAllocSize) return false;
	if (this->distrReallocJump != value.distrReallocJump) return false;
	//trace
	if (this->traceEnabled != value.traceEnabled) return false;
	//info
	if (this->infoHidden != value.infoHidden) return false;
	//exe
	if (this->exe != value.exe) return false;
	if (this->childs != value.childs) return false;
	if (this->enabled != value.enabled) return false;
	if (this->filterRanks != value.filterRanks) return false;
	//dump
	if (this->dumpOnSignal != value.dumpOnSignal) return false;
	if (this->dumpAfterSeconds != value.dumpAfterSeconds) return false;
	if (this->dumpOnSysFullAt != value.dumpOnSysFullAt) return false;
	if (this->dumpOnAppUsingRss != value.dumpOnAppUsingRss) return false;
	if (this->dumpOnAppUsingVirt != value.dumpOnAppUsingVirt) return false;
	if (this->dumpOnAppUsingReq != value.dumpOnAppUsingReq) return false;
	if (this->dumpOnThreadStackUsing != value.dumpOnThreadStackUsing) return false;
	if (this->dumpWatchDog != value.dumpWatchDog) return false;
	if (this->dumpOnAllocCount != value.dumpOnAllocCount) return false;
	//nm
	if (this->toolsNm != value.toolsNm) return false;
	if (this->toolsNmMaxSize != value.toolsNmMaxSize) return false;
	
	return true;
}

/**********************************************************/
/**
 * Load values from string, mostly to be used from MALT_OPTION environment variable.
 * 
 * It expect string format like :
 * 
 * SEC1:NAME1=VALUE1;SEC2:NAME2=VALUE2;
 * 
 * @param value Define the string to load as a config file.
**/
void Options::loadFromString ( const char* value )
{
	//trivial
	if (value == NULL)
		return;

	//create fake dictionary
	dictionary * dic = dictionary_new(10);
	
	//copy string
	char * dump = strdup(value);
	
	//loop on separators ';'
	char * cur = dump;
	while (*cur != '\0')
	{
		//remind start
		char * start = cur;
		char * sep = NULL;
		
		//search ';' or '\0'
		while (*cur != ';' && *cur != '\0')
		{
			if (*cur == '=')
				sep = cur;
			cur++;
		}
		
		//skip to next
		if (cur == start)
		{
			cur++;
			continue;
		}
		
		//is end
		bool isEnd = (*cur == '\0');
		if (sep == NULL) {
			fprintf(stderr, "Invalid string format to setup option : '%s', expect SECTION:NAME=VALUE.\n", start);
			exit(1);
		}

		//cut end
		*cur = '\0';

		//cut on separator
		*sep = '\0';
		sep++;

		//validate
		if (checkDeprecated(start)) {
			exit(1);
		}
		if (validateOptionName(start) == false) {
			fprintf(stderr, "Invalid option given to MALT : %s !\n", start);
			exit(1);
		}

		//rename
		const char * key = sep;
		
		//setup in INI
		IniParserHelper::setEntry(dic,start,key);
		
		//move
		if (isEnd == false)
			cur++;
	}
	
	//load
	this->loadFromIniDic(dic);

	//free
	iniparser_freedict(dic);
	free(dump);
}

/**********************************************************/
/**
 * Internal function to load options from iniDic.
**/
void Options::loadFromIniDic ( dictionary* iniDic )
{
	//errors
	assert(iniDic != NULL);

	//load
	OptionsMeta meta(*this);
	meta.loadIni(iniDic);
}

/**********************************************************/
/**
 * Function to load options from a config file in INI format.
**/
void Options::loadFromFile(const char* fname)
{
	//load ini dic
	dictionary * iniDic;
	assert(fname != NULL);
	iniDic = iniparser_load(fname);
	
	//errors
	assumeArg(iniDic != NULL,"Failed to load config file : %1 !").arg(fname);
	
	//load
	loadFromIniDic(iniDic);
	
	//free dic
	iniparser_freedict(iniDic);
	
	//TODO apply getenv MALT_OPTIONS to override here and add "envOverride" parameter to enable it from caller
}

/**********************************************************/
/**
 * Helper function to convert the options to JSON output format and dump it
 * into the MALT output profile.
**/
void convertToJson(htopml::JsonState & json,const Options & value)
{
	OptionsMeta meta(const_cast<Options&>(value));
	meta.dumpAsJson(json);

}

/**********************************************************/
/**
 * Helper to dump the config as INI file.
**/
void Options::dumpConfig(const char* fname)
{
	OptionsMeta meta(*this);
	meta.dumpConfig(fname);
}

/**********************************************************/
/**
 * Internal function to split strings on ':' and extract the section name.
**/
std::string IniParserHelper::extractSectionName ( const char * key )
{
	std::string tmp;
	int i = 0;
	while (key[i] != ':' && key[i] != '\0')
		tmp += key[i++];
	return tmp;
}

/**********************************************************/
/**
 * Need to be call once after malloc is available.
**/
Options& initGlobalOptions ( void )
{
	//error
	//assume (gblOptions == NULL,"initGlobalOptions was used previously, gblOptions is already init ! ");
	if (gblOptions != nullptr)
		return *gblOptions;
	gblOptions = new Options();
	return *gblOptions;
}

/**********************************************************/
/**
 * Convert verbosity level from string.
**/
Verbosity verbosityFromString(const std::string & value)
{
	//search match
	for (int i = 0 ; i < MALT_VERBOSITY_COUNT ; i++)
		if (value == cstVerbosityLevels[i])
			return (Verbosity)i;

	//error
	MALT_FATAL_ARG("Invalid verbosity level: '%1'").arg(value).end();
	return MALT_VERBOSITY_DEFAULT;
}

/**********************************************************/
StackMode stackModeFromString(const std::string & value)
{
	if (value == "backtrace")
		return STACK_MODE_BACKTRACE;
	else if (value == "enter-exit")
		return STACK_MODE_ENTER_EXIT_FUNC;
	else if (value == "user")
		return STACK_MODE_USER;
	else if (value == "none")
		return STACK_MODE_NONE;
	else if (value == "python")
		return STACK_MODE_PYTHON;
	else
		MALT_FATAL_ARG("Invalid stack mode : %1").arg(value).end();

	//default
	return STACK_MODE_NONE;
}

/**********************************************************/
/**
 * Convert verbosity level to string.
**/
const char * verbosityToString(Verbosity value)
{
	assert(value < MALT_VERBOSITY_COUNT);
	return cstVerbosityLevels[value];
}

/**********************************************************/
Verbosity iniparser_getverbosity(dictionary * dic, const char * key, Verbosity notFound)
{
	//tansmit
	const char * tmp = iniparser_getstring(dic,key,(char*)verbosityToString(notFound));

	//convert back
	Verbosity res = verbosityFromString(tmp);

	//ok
	return res;
}

/**********************************************************/
/**
 * Dump to stream to be used for json output.
**/
std::ostream & operator << (std::ostream & out, Verbosity value)
{
	out << verbosityToString(value);
	return out;
}

/**********************************************************/
/**
 * In order to display an error if the user didn't gave the right name
 * in the command line.
 * @param value The option name to validate.
 * @return True if valid, false otherwise.
 */
bool validateOptionName(const std::string & value)
{
	Options options;
	OptionsMeta meta(options);
	return meta.isValidGroupKey(value);
}

/**********************************************************/
/**
 * Check if some option has changed their name to avert the user.
 */
bool checkDeprecated(const std::string & value)
{
	if (value == "time:linear_index") {
		fprintf(stderr, "Option 'time:linear_index' as been renamed as `time:linear`");
		return true;
	} else if (value == "stack:addr2lineBucket") {
		fprintf(stderr, "Option 'stack:addr2lineBucket' as been renamed as `addr2line:bucket`");
		return true;
	} else if (value == "stack:addr2lineThreads") {
		fprintf(stderr, "Option 'stack:addr2lineThreads' as been renamed as `addr2line:threads`");
		return true;
	} else if (value == "stack:addr2lineHuge") {
		fprintf(stderr, "Option 'stack:addr2lineHuge' as been renamed as `addr2line:huge`");
		return true;
	} else if (value == "stack:sampling") {
		fprintf(stderr, "Option 'stack:sampling' as been renamed as `sampling:enabled`");
		return true;
	} else if (value == "stack:samplingBw") {
		fprintf(stderr, "Option 'stack:samplingBw' as been renamed as `sampling:volume`");
		return true;
	} else if (value == "stack:samplingCnt") {
		fprintf(stderr, "Option 'stack:samplingCnt' as been renamed as `sampling:count`");
		return true;
	}
	return false;
}

/**********************************************************/
OptionsMeta::OptionsMeta(Options & value)
{
	//load values for time profiling
	this->add("time", "enabled", value.timeProfileEnabled).setDoc("Enable time profiles.");
	this->add("time", "points", value.timeProfilePoints).setDoc("Number of points to keep in the cart.");
	this->add("time", "linear", value.timeProfileLinear).setDoc("Use operation index instead of time.");
	
	//python
	this->add("python", "instru", value.pythonInstru).setDoc("Enable of disable python instrumentation.");
	this->add("python", "mix", value.pythonMix).setDoc("Mix C stack with the python ones to get a uniq tree instread of two distincts (note it adds overhead).");
	this->add("python", "stack", value.pythonStack).setDoc("Select the Python stack instrumentation mode (backtrace, enter-exit, none).");;
	this->add("python", "obj", value.pythonObj).setDoc("Instrument of not the OBJECT allocator domain of python.");
	this->add("python", "mem", value.pythonMem).setDoc("Instrument of not the MEM allocator domain of python.");
	this->add("python", "raw", value.pythonRaw).setDoc("Instrument of not the RAW allocator domain of python.");
	this->add("python", "hide-imports", value.pythonHideImports).setDoc("Do not keep stacks for the python import related allocations.");
	this->add("python", "mode", value.pythonMode).setDoc("Configure the python instrumentation mode ('profile' or 'trace').");

	//c
	this->add("c", "malloc", value.cMalloc).setDoc("Instrument the C memory allocator.");
	this->add("c", "mmap", value.cMmap).setDoc("Instrument the C direct mmap calls done by the app.");

	//load values for stack profiling
	this->add("stack", "resolve", value.stackResolve).setDoc("Automatically resolve symbols with addr2line at exit.");
	this->add("stack", "enabled", value.stackProfileEnabled).setDoc("Enable stack profiles.");
	this->add("stack", "libunwind", value.stackLibunwind).setDoc("Enable of disable usage of libunwind to backtrace.");
	this->add("stack", "mode", value.stackMode).setDoc("Define the stack tracking mode (enter-exit, backtrace, python, none)");
	this->add("stack", "skip", value.stackSkip).setDoc("Number of stack frame to skip in order to cut at malloc level.");

	//addr2line
	this->add("addr2line", "bucket", value.stackAddr2lineBucket).setDoc("Handle the addr2line calls by buckets and treat each bucket in parallel.");
	this->add("addr2line", "threads", value.stackAddr2lineThreads).setDoc("Number of threasd to use to call addr2line in parallel.");
	this->add("addr2line", "huge", value.stackAddr2lineHuge).setDoc("For larger elf files, do not treat them in parallel nor buckets.");

	//sampling
	this->add("sampling", "enabled", value.stackSampling).setDoc("Sample and instrument only some stack.");
	this->add("sampling", "volume", value.stackSamplingBw).setDoc("Instrument the stack when seen passed 4K-3 bytes of alloc requests (ideally should be prime number).");
	this->add("sampling", "count", value.stackSamplingCnt).setDoc("Instrument the stack when seen passed X alloc requests (ideally should be prime number).");;
	
	//load values for output
	this->add("output", "name", value.outputName).setDoc("base name for output, %1 = exe, %2 = PID, %3 = extension.");
	this->add("output", "indent", value.outputIndent).setDoc("indent the output profile files");
	this->add("output", "json", value.outputJson).setDoc("Enable json output.");
	this->add("output", "lua", value.outputLua).setDoc("Enable lua output.");
	this->add("output", "callgrind", value.outputCallgrind).setDoc("Enable callgrind output.");
	this->add("output", "config", value.outputDumpConfig).setDoc("Dump current config.");
	this->add("output", "verbosity", value.outputVerbosity).setDoc("Malt verbosity level (silent, default, verbose).");
	this->add("output", "stack-tree", value.outputStackTree).setDoc("Store the call tree as a tree (smaller file, but need conversion in the reader)");
	this->add("output", "loop-suppress", value.outputLoopSuppress).setDoc("Simplify recursive loop calls to get smaller profile file if too big");

	//max stack
	this->add("max-stack", "enabled", value.maxStackEnabled).setDoc("enable of disable strack size tracking (require -finstrument-functions).");

	//maps
	this->add("distr", "alloc-size", value.distrAllocSize).setDoc("Generate distribution of allocation size.");
	this->add("distr", "realloc-jump", value.distrReallocJump).setDoc("Generate distribution of realloc jumps.");
	
	//trace
	this->add("trace", "enabled", value.traceEnabled).setDoc("Enable dumping allocation event tracing (not yet used by GUI).");
	
	//info
	this->add("info", "hidden", value.infoHidden).setDoc("Try to hide possible sensible names from profile (exe, hostname...).");

	//filter
	this->add("filter", "exe", value.exe).setDoc("Only apply malt on given exe (empty for all).");
	this->add("filter", "childs", value.childs).setDoc("Instrument child processes or not.");
	this->add("filter", "enabled", value.enabled).setDoc("Enable or disable MALT when threads start.");
	this->add("filter", "ranks", value.filterRanks).setDoc("Instrument only the given ranks from list as : 1,2-4,6");

	//dump
	this->add("dump", "on-signal", value.dumpOnSignal).setDoc("Dump on signal. Can be comma separated list from SIGINT, SIGUSR1, SIGUSR2... help, avail (limited to only one dump).");
	this->add("dump", "after-seconds", value.dumpAfterSeconds).setDoc("Dump after X seconds (limited to only one time).");
	this->add("dump", "on-sys-full-at", value.dumpOnSysFullAt).setDoc("Dump when system memory become full at x%, xG, xM, xK, x  (empty to disable).");
	this->add("dump", "on-app-using-rss", value.dumpOnAppUsingRss).setDoc("Dump when RSS of the app reach the given limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-app-using-virt", value.dumpOnAppUsingVirt).setDoc("Dump when Virtual Memory of the app reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-app-using-req", value.dumpOnAppUsingReq).setDoc("Dump when Requested Memory of the app reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-thread-stack-using", value.dumpOnThreadStackUsing).setDoc("Dump when one stack reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-alloc-count", value.dumpOnAllocCount).setDoc("Dump when number of allocations reach limit in G, M, K (empty to disable).");
	this->add("dump", "watch-dog", value.dumpWatchDog).setDoc("Run an active thread spying continuouly the memory of the app, not only sometimes.");

	//tools
	this->add("tools", "nm", value.toolsNm).setDoc("Enable usage of NM to find the source locatoin of the global variables.");
	this->add("tools", "nm-max-size", value.toolsNmMaxSize).setDoc("Do not call nm on .so larger than 50 MB to limit the profile dump overhead.");
}

/**********************************************************/
OptionsMeta::~OptionsMeta(void)
{
	for (auto & it : this->meta) {
		delete it.second;
		it.second = nullptr;
	}
	this->meta.clear();
}

/**********************************************************/
void OptionsMeta::dumpConfig(const char * fname) const
{
	//create dic
	assert(fname != NULL);
	dictionary * dic = dictionary_new(10);
	
	//fill
	for (const auto & it : this->meta)
		it.second->saveToIni(dic);

	//write
	FILE * fp = fopen(fname,"w");
	assumeArg(fp != NULL,"Failed to write dump of config file into %1 : %2 !").arg(fname).argStrErrno().end();
	iniparser_dump_ini(dic,fp);
	fclose(fp);
	
	//free
	iniparser_freedict(dic);
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, StackMode mode);
std::ostream & operator<<(std::ostream & out, PythonMode mode);

/**********************************************************/
int setByString(int &number, const std::string & value)
{
	number = atoi(value.c_str());
	return number;
}

/**********************************************************/
size_t setByString(size_t &number, const std::string & value)
{
	sscanf(value.c_str(), "%zu", &number);
	return number;
}

/**********************************************************/
Verbosity setByString(Verbosity & mode, const std::string & value)
{
	if (value == "default")
		mode = MALT_VERBOSITY_DEFAULT;
	else if (value == "verbose")
		mode = MALT_VERBOSITY_VERBOSE;
	else if (value == "silent")
		mode = MALT_VERBOSITY_SILENT;
	else
		MALT_FATAL_ARG("Invalid verbosity mode : '%1'! Supported : default | silent | verbose.").arg(value).end();
	return mode;
}

/**********************************************************/
StackMode setByString(StackMode & mode, const std::string & value)
{
	//switches
	if (value.c_str() == NULL)
	{
		mode = STACK_MODE_BACKTRACE;
	} else if (value == "backtrace" || value == "") {
		mode = STACK_MODE_BACKTRACE;
	} else if (value == "libunwind") {
		mode = STACK_MODE_BACKTRACE;
	} else if (value == "enter-exit") {
		mode = STACK_MODE_ENTER_EXIT_FUNC;
	} else if (value == "none") {
		mode = STACK_MODE_NONE;
	} else if (value == "python") {
		mode = STACK_MODE_PYTHON;
	} else {
		MALT_FATAL_ARG("Invalid mode in MALT_STACK environnement variable : '%1'! Supported : backtrace | enter-exit | none.").arg(value).end();
	}
	return mode;
}

/**********************************************************/
PythonMode setByString(PythonMode & mode, const std::string & value)
{
	if (value == "profile")
		mode = PYTHON_MODE_PROFILE;
	else if (value == "trace")
		mode = PYTHON_MODE_TRACE;
	else
		MALT_FATAL_ARG("Invalid mode in python mode : '%1'! Supported : profile | trace.").arg(value).end();
	return mode;
}

/**********************************************************/
void OptionsMeta::dumpAsJson(htopml::JsonState & json) const
{
	//vars
	std::set<std::string> done;

	//open
	json.openStruct();

	//loop on all
	for (const auto & it : this->meta) {
		//current group
		const std::string & group = it.second->getGroup();

		//if not yet done
		if (done.find(group) == done.end()) {
			//open
			json.openFieldStruct(group.c_str());

			//inject all in group
			for (const auto & it : this->meta) {
				if (it.second->getGroup() == group)
					it.second->convertToJson(json);
			}

			//close
			json.closeFieldStruct(group.c_str());

			//done
			done.insert(group);
		}
	}

	//close
	json.closeStruct();
}

/**********************************************************/
void OptionsMeta::loadIni(dictionary* iniDic)
{
	for (auto & it : this->meta)
		it.second->loadFromIni(iniDic);
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, StackMode mode)
{
	switch(mode)
	{
		case STACK_MODE_BACKTRACE:
			out << "backtrace";
			break;
		case STACK_MODE_ENTER_EXIT_FUNC:
			out << "enter-exit";
			break;
		case STACK_MODE_USER:
			out << "user";
			break;
		case STACK_MODE_NONE:
			out << "none";
			break;
		case STACK_MODE_PYTHON:
			out << "python";
			break;
	}
	return out;
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, PythonMode mode)
{
	switch(mode)
	{
		case PYTHON_MODE_PROFILE:
			out << "profile";
			break;
		case PYTHON_MODE_TRACE:
			out << "trace";
			break;
	}
	return out;
}

/**********************************************************/
void OptionsMeta::dump(std::ostream & out) const
{
	for (const auto & it : this->meta) {
		out << it.first << "=" << it.second->toString() << std::endl;
	}
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, const Options & options)
{
	OptionsMeta meta(const_cast<Options &>(options));
	meta.dump(out);
	return out;
}

/**********************************************************/
bool OptionsMeta::isValidGroupKey(const std::string & value) const
{
	return this->meta.find(value) != this->meta.end();
}

/**********************************************************/
void convertToJson(htopml::JsonState & json,const StackMode & value)
{
	std::stringstream buffer;
	buffer << value;
	convertToJson(json, buffer.str());
}

/**********************************************************/
void convertToJson(htopml::JsonState & json,const PythonMode & value)
{
	std::stringstream buffer;
	buffer << value;
	convertToJson(json, buffer.str());
}

/**********************************************************/
void convertToJson(htopml::JsonState & json,const Verbosity & value)
{
	std::stringstream buffer;
	buffer << value;
	convertToJson(json, buffer.str());
}

}

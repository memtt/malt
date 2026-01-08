/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Options.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2026
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
	if (stack.enabled != value.stack.enabled) return false;
	if (stack.resolve != value.stack.resolve) return false;
	if (stack.mode != value.stack.mode) return false;
	if (stack.libunwind != value.stack.libunwind) return false;
	if (stack.skip != value.stack.skip) return false;
	if (addr2line.bucket != value.addr2line.bucket) return false;
	if (addr2line.threads != value.addr2line.threads) return false;
	if (addr2line.huge != value.addr2line.huge) return false;
	if (sampling.enabled != value.sampling.enabled) return false;
	if (sampling.volume != value.sampling.volume) return false;
	if (sampling.count != value.sampling.count) return false;
	//python
	if (python.enabled != value.python.enabled) return false;
	if (python.stack != value.python.stack) return false;
	if (python.mix != value.python.mix) return false;
	if (python.obj != value.python.obj) return false;
	if (python.mem != value.python.mem) return false;
	if (python.raw != value.python.raw) return false;
	if (python.hideImports != value.python.hideImports) return false;
	if (python.mode != value.python.mode) return false;
	//c
	if (c.malloc != value.c.malloc) return false;
	if (c.mmap != value.c.mmap) return false;
	if (c.gpuMalloc != value.c.gpuMalloc) return false;
	//time
	if (this->time.enabled != value.time.enabled) return false;
	if (this->time.points != value.time.points) return false;
	if (this->time.linear != value.time.linear) return false;
	//output
	if (this->output.name != value.output.name) return false;
	if (this->output.indent != value.output.indent) return false;
	if (this->output.json != value.output.json) return false;
	if (this->output.lua != value.output.lua) return false;
	if (this->output.callgrind != value.output.callgrind) return false;
	if (this->output.config != value.output.config) return false;
	if (this->output.verbosity != value.output.verbosity)  return false;
	if (this->output.stackTree != value.output.stackTree) return false;
	if (this->output.loopSuppress != value.output.loopSuppress) return false;
	//max stack
	if (this->maxStack.enabled != value.maxStack.enabled) return false;
	//maps
	if (this->distr.allocSize != value.distr.allocSize) return false;
	if (this->distr.reallocJump != value.distr.reallocJump) return false;
	//trace
	if (this->trace.enabled != value.trace.enabled) return false;
	//info
	if (this->info.hidden != value.info.hidden) return false;
	//exe
	if (this->filter.exe != value.filter.exe) return false;
	if (this->filter.childs != value.filter.childs) return false;
	if (this->filter.enabled != value.filter.enabled) return false;
	if (this->filter.ranks != value.filter.ranks) return false;
	//dump
	if (this->dump.onSignal != value.dump.onSignal) return false;
	if (this->dump.afterSeconds != value.dump.afterSeconds) return false;
	if (this->dump.onSysFullAt != value.dump.onSysFullAt) return false;
	if (this->dump.onAppUsingRss != value.dump.onAppUsingRss) return false;
	if (this->dump.onAppUsingVirt != value.dump.onAppUsingVirt) return false;
	if (this->dump.onAppUsingReq != value.dump.onAppUsingReq) return false;
	if (this->dump.onThreadStackUsing != value.dump.onThreadStackUsing) return false;
	if (this->dump.watchDog != value.dump.watchDog) return false;
	if (this->dump.onAllocCount != value.dump.onAllocCount) return false;
	//nm
	if (this->tools.nm != value.tools.nm) return false;
	if (this->tools.nmMaxSize != value.tools.nmMaxSize) return false;
	
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
		fprintf(stderr, "Option 'time:linear_index' as been renamed as `time:linear`\n");
		return true;
	} else if (value == "stack:addr2lineBucket") {
		fprintf(stderr, "Option 'stack:addr2lineBucket' as been renamed as `addr2line:bucket`\n");
		return true;
	} else if (value == "stack:addr2lineThreads") {
		fprintf(stderr, "Option 'stack:addr2lineThreads' as been renamed as `addr2line:threads`\n");
		return true;
	} else if (value == "stack:addr2lineHuge") {
		fprintf(stderr, "Option 'stack:addr2lineHuge' as been renamed as `addr2line:huge`\n");
		return true;
	} else if (value == "stack:sampling") {
		fprintf(stderr, "Option 'stack:sampling' as been renamed as `sampling:enabled`\n");
		return true;
	} else if (value == "stack:samplingBw") {
		fprintf(stderr, "Option 'stack:samplingBw' as been renamed as `sampling:volume`\n");
		return true;
	} else if (value == "stack:samplingCnt") {
		fprintf(stderr, "Option 'stack:samplingCnt' as been renamed as `sampling:count`\n");
		return true;
	} else if (value == "python:instru") {
		fprintf(stderr, "Option 'python:instru' as been renamed as `python:enabled`\n");
		return true;
	}
	return false;
}

/**********************************************************/
OptionsMeta::OptionsMeta(Options & value)
{
	//load values for time profiling
	this->add("time", "enabled", value.time.enabled).setDoc("Enable time profiles.");
	this->add("time", "points", value.time.points).setDoc("Number of points to keep in the cart.");
	this->add("time", "linear", value.time.linear).setDoc("Use operation index instead of time.");
	
	//python
	this->add("python", "instru", value.python.enabled).setDoc("Enable of disable python instrumentation.");
	this->add("python", "mix", value.python.mix).setDoc("Mix C stack with the python ones to get a uniq tree instread of two distincts (note it adds overhead).");
	this->add("python", "stack", value.python.stack).setDoc("Select the Python stack instrumentation mode (backtrace, enter-exit, none).");;
	this->add("python", "obj", value.python.obj).setDoc("Instrument of not the OBJECT allocator domain of python.");
	this->add("python", "mem", value.python.mem).setDoc("Instrument of not the MEM allocator domain of python.");
	this->add("python", "raw", value.python.raw).setDoc("Instrument of not the RAW allocator domain of python.");
	this->add("python", "hide-imports", value.python.hideImports).setDoc("Do not keep stacks for the python import related allocations.");
	this->add("python", "mode", value.python.mode).setDoc("Configure the python instrumentation mode ('profile' or 'trace').");

	//c
	this->add("c", "malloc", value.c.malloc).setDoc("Instrument the C memory allocator.");
	this->add("c", "mmap", value.c.mmap).setDoc("Instrument the C direct mmap calls done by the app.");
	this->add("c", "gpu-malloc", value.c.gpuMalloc).setDoc("Instrument the C calls to GPU malloc (cudaMalloc, hipMalloc...).");

	//load values for stack profiling
	this->add("stack", "resolve", value.stack.resolve).setDoc("Automatically resolve symbols with addr2line at exit.");
	this->add("stack", "enabled", value.stack.enabled).setDoc("Enable stack profiles.");
	this->add("stack", "libunwind", value.stack.libunwind).setDoc("Enable of disable usage of libunwind to backtrace.");
	this->add("stack", "mode", value.stack.mode).setDoc("Define the stack tracking mode (enter-exit, backtrace, python, none)");
	this->add("stack", "skip", value.stack.skip).setDoc("Number of stack frame to skip in order to cut at malloc level.");

	//addr2line
	this->add("addr2line", "bucket", value.addr2line.bucket).setDoc("Handle the addr2line calls by buckets and treat each bucket in parallel.");
	this->add("addr2line", "threads", value.addr2line.threads).setDoc("Number of threasd to use to call addr2line in parallel.");
	this->add("addr2line", "huge", value.addr2line.huge).setDoc("For larger elf files, do not treat them in parallel nor buckets.");

	//sampling
	this->add("sampling", "enabled", value.sampling.enabled).setDoc("Sample and instrument only some stack.");
	this->add("sampling", "volume", value.sampling.volume).setDoc("Instrument the stack when seen passed 4K-3 bytes of alloc requests (ideally should be prime number).");
	this->add("sampling", "count", value.sampling.count).setDoc("Instrument the stack when seen passed X alloc requests (ideally should be prime number).");;
	
	//load values for output
	this->add("output", "name", value.output.name).setDoc("base name for output, %1 = exe, %2 = PID, %3 = extension.");
	this->add("output", "indent", value.output.indent).setDoc("indent the output profile files");
	this->add("output", "json", value.output.json).setDoc("Enable json output.");
	this->add("output", "lua", value.output.lua).setDoc("Enable lua output.");
	this->add("output", "callgrind", value.output.callgrind).setDoc("Enable callgrind output.");
	this->add("output", "config", value.output.config).setDoc("Dump current config.");
	this->add("output", "verbosity", value.output.verbosity).setDoc("Malt verbosity level (silent, default, verbose).");
	this->add("output", "stack-tree", value.output.stackTree).setDoc("Store the call tree as a tree (smaller file, but need conversion in the reader)");
	this->add("output", "loop-suppress", value.output.loopSuppress).setDoc("Simplify recursive loop calls to get smaller profile file if too big");

	//max stack
	this->add("max-stack", "enabled", value.maxStack.enabled).setDoc("enable of disable strack size tracking (require -finstrument-functions).");

	//maps
	this->add("distr", "alloc-size", value.distr.allocSize).setDoc("Generate distribution of allocation size.");
	this->add("distr", "realloc-jump", value.distr.reallocJump).setDoc("Generate distribution of realloc jumps.");
	
	//trace
	this->add("trace", "enabled", value.trace.enabled).setDoc("Enable dumping allocation event tracing (not yet used by GUI).");
	
	//info
	this->add("info", "hidden", value.info.hidden).setDoc("Try to hide possible sensible names from profile (exe, hostname...).");

	//filter
	this->add("filter", "exe", value.filter.exe).setDoc("Only apply malt on given exe (empty for all).");
	this->add("filter", "childs", value.filter.childs).setDoc("Instrument child processes or not.");
	this->add("filter", "enabled", value.filter.enabled).setDoc("Enable or disable MALT when threads start.");
	this->add("filter", "ranks", value.filter.ranks).setDoc("Instrument only the given ranks from list as : 1,2-4,6");

	//dump
	this->add("dump", "on-signal", value.dump.onSignal).setDoc("Dump on signal. Can be comma separated list from SIGINT, SIGUSR1, SIGUSR2... help, avail (limited to only one dump).");
	this->add("dump", "after-seconds", value.dump.afterSeconds).setDoc("Dump after X seconds (limited to only one time).");
	this->add("dump", "on-sys-full-at", value.dump.onSysFullAt).setDoc("Dump when system memory become full at x%, xG, xM, xK, x  (empty to disable).");
	this->add("dump", "on-app-using-rss", value.dump.onAppUsingRss).setDoc("Dump when RSS of the app reach the given limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-app-using-virt", value.dump.onAppUsingVirt).setDoc("Dump when Virtual Memory of the app reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-app-using-req", value.dump.onAppUsingReq).setDoc("Dump when Requested Memory of the app reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-thread-stack-using", value.dump.onThreadStackUsing).setDoc("Dump when one stack reach limit in %, G, M, K (empty to disable).");
	this->add("dump", "on-alloc-count", value.dump.onAllocCount).setDoc("Dump when number of allocations reach limit in G, M, K (empty to disable).");
	this->add("dump", "watch-dog", value.dump.watchDog).setDoc("Run an active thread spying continuouly the memory of the app, not only sometimes.");

	//tools
	this->add("tools", "nm", value.tools.nm).setDoc("Enable usage of NM to find the source locatoin of the global variables.");
	this->add("tools", "nm-max-size", value.tools.nmMaxSize).setDoc("Do not call nm on .so larger than 50 MB to limit the profile dump overhead.");
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

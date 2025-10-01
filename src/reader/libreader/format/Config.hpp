/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Config.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_CONFIG_HPP
#define MALT_FORMAT_CONFIG_HPP

/**********************************************************/
#include <string>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct Config
{
	struct {
		bool enabled;
		size_t points;
		bool linear;
	} time;
	struct {
		bool enabled;
		std::string mode;
		bool resolve;
		bool libunwind;
		size_t stackSkip;
		size_t addr2lineBucket;
		size_t addr2lineThreads;
		bool sampling;
		size_t samplingBw;
		size_t samplingCnt;
	} stack;
	struct {
		bool instru;
		bool mix;
		std::string stack;
		bool obj;
		bool mem;
		bool raw;
		bool hideImports;
		std::string mode;
	} python;
	struct {
		bool malloc;
		bool mmap;
	} c;
	struct {
		bool callgrind;
		bool dumpConfig;
		bool index;
		bool json;
		bool lua;
		std::string name;
		std::string verbosity;
		bool stackTree;
		bool loopSuppress;
	} output;
	struct {
		bool enabled;
	} maxStack;
	struct {
		bool allocSize;
		bool reallocJump;
	} distr;
	struct {
		bool hidden;
	} info;
	struct {
		std::string exe;
		bool childs;
		bool enabled;
		std::string ranks;
	} filter;
	struct {
		std::string onSignal;
		size_t afterSeconds;
		std::string onSysFullAt;
		std::string onAppUsingRss;
		std::string onAppUsingVirt;
		std::string onAppUsingReq;
		std::string onThreadStackUsing;
		std::string onAllocCount;
		bool watchDog;
	} dump;
	struct {
		bool nm;
		std::string nmMaxSize;
	} tools;
};

/**********************************************************/
void from_json(const JsonIn & json, Config & config);
void to_json(nlohmann::json & json, const Config & config);

}

#endif //MALT_FORMAT_CONFIG_HPP

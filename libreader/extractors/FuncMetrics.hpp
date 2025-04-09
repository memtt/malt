/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/CallTreeAdapter.js
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat - 2022 - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
***********************************************************/

/**********************************************************/
#ifndef MALT_READER_FUNC_METRIC_HPP
#define MALT_READER_FUNC_METRIC_HPP

/**********************************************************/
#include <map>
#include <string>
#include <functional>
#include "Extractor.hpp"
#include "../format/Stacks.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
enum SortingOrder
{
	SORT_ASC,
	SORT_DESC
};

/**********************************************************/
enum RefMode
{
	REF_MODE_SUM,
	REF_MODE_MAX,
};

/**********************************************************/
struct MaltMetric
{
	std::string name;
	std::function<size_t(const MALTFormat::StackInfos & infos)> extractor;
	std::function<std::string(size_t value)> formater;
	SortingOrder defaultOrder;
	RefMode ref;
};

/**********************************************************/
class MaltFuncMetrics
{
	public:
		MaltFuncMetrics(void);
		~MaltFuncMetrics(void);
		std::vector<std::string> getMetricNames(void) const;
		const std::map<std::string, MaltMetric> & getMetrics(void) const;
		size_t getValue(const FlatProfileValue & dataElement, const std::string & metricName, bool inclusive) const;
		std::string getFormattedValue(const FlatProfileValue & dataElement, const std::string & metricName, bool inclusive) const;
	private:
		void fillMetrics(void);
	private:
		std::map<std::string, MaltMetric> metrics;
};

}

#endif //MALT_READER_FUNC_METRIC_HPP

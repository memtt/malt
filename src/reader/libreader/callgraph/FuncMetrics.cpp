/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/FuncMetrics.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "extractors/ExtractorHelpers.hpp"
#include "FuncMetrics.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
MaltFuncMetrics::MaltFuncMetrics(void)
{
	this->fillMetrics();
}

/**********************************************************/
MaltFuncMetrics::~MaltFuncMetrics(void)
{

}

/**********************************************************/
void MaltFuncMetrics::fillMetrics(void)
{
	// alloc
	this->metrics["alloc.sum"] = {
		"Allocated mem.",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.sum;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["alloc.count"] = {
		"Allocated count",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["alloc.min"] = {
		"Allocated min",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.min;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_ASC,
		REF_MODE_MAX,
	};
	this->metrics["alloc.mean"] = {
		"Allocate mean",
		[](const MALTFormat::StackInfos & infos) {return (double)infos.alloc.sum / (double)infos.alloc.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_ASC,
		REF_MODE_MAX,
	};
	this->metrics["alloc.max"] = {
		"Allocated max",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.max;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_ASC,
		REF_MODE_MAX,
	};

	// free
	this->metrics["free.sum"] = {
		"Freed mem.",
		[](const MALTFormat::StackInfos & infos) {return infos.free.sum;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["free.count"] = {
		"Free count",
		[](const MALTFormat::StackInfos & infos) {return infos.free.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};

	//mem ops
	this->metrics["memops"] = {
		"Memory ops.",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.count + infos.free.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};

	//peaks
	this->metrics["peakmem.local"] = {
		"Local peak",
		[](const MALTFormat::StackInfos & infos) {return infos.maxAliveReq;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};

	//peaks
	this->metrics["peakmem.global"] = {
		"Global peak",
		[](const MALTFormat::StackInfos & infos) {return infos.globalPeak;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};

	//peaks
	this->metrics["peakmemGPU.local"] = {
		"GPU Local peak",
		[](const MALTFormat::StackInfos & infos) {return infos.maxAliveReqGPU;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};

	//peaks
	this->metrics["peakmemGPU.global"] = {
		"GPU Global peak",
		[](const MALTFormat::StackInfos & infos) {return infos.globalPeakGPU;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};

	//leaks
	this->metrics["leaks"] = {
		"Leaks",
		[](const MALTFormat::StackInfos & infos) {return infos.aliveReq;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};

	//lifetime
	this->metrics["lifetime.max"] = {
		"Max lifetime",
		[](const MALTFormat::StackInfos & infos) {return infos.lifetime.max;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};
	this->metrics["lifetime.min"] = {
		"Min lifetime",
		[](const MALTFormat::StackInfos & infos) {return infos.lifetime.max;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_ASC,
		REF_MODE_MAX,
	};

	//recycling
	this->metrics["recycling.ratio"] = {
		"Recycling ratio",
		[](const MALTFormat::StackInfos & infos) {return infos.alloc.count == 0 ? 1 : (double)infos.alloc.sum / (double)infos.maxAliveReq;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_MAX,
	};

	//realloc
	this->metrics["realloc.count"] = {
		"Realloc count",
		[](const MALTFormat::StackInfos & infos) {return infos.reallocCount;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["realloc.sum"] = {
		"Realloc sum",
		[](const MALTFormat::StackInfos & infos) {return infos.reallocSumDelta;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};

	//mmap
	this->metrics["mmap.count"] = {
		"Mmap count",
		[](const MALTFormat::StackInfos & infos) {return infos.mmap.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["mmap.sum"] = {
		"Mmap sum",
		[](const MALTFormat::StackInfos & infos) {return infos.mmap.sum;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["munmap.count"] = {
		"Munmap count",
		[](const MALTFormat::StackInfos & infos) {return infos.munmap.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["munmap.sum"] = {
		"Munmap sum",
		[](const MALTFormat::StackInfos & infos) {return infos.munmap.sum;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};

	this->metrics["gpuAlloc.sum"] = {
		"GPU Allocated mem.",
		[](const MALTFormat::StackInfos & infos) {return infos.gpuAlloc.sum;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"B",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
	this->metrics["gpuAlloc.count"] = {
		"GPU Allocated count",
		[](const MALTFormat::StackInfos & infos) {return infos.gpuAlloc.count;},
		[](size_t value) {return ExtractorHelpers::humanReadable(value,1,"",false);},
		SORT_DESC,
		REF_MODE_SUM,
	};
}

/**********************************************************/
std::vector<std::string> MaltFuncMetrics::getMetricNames(void) const
{
	std::vector<std::string> res;
	res.reserve(this->metrics.size());
	for (const auto & it : this->metrics)
		res.push_back(it.first);
	return res;
}

/**********************************************************/
const std::map<std::string, MaltMetric> & MaltFuncMetrics::getMetrics(void) const
{
	return this->metrics;
}

/**********************************************************/
size_t MaltFuncMetrics::getValue(const FlatProfileValue & dataElement, const std::string & metricName, bool inclusive) const
{
	//get metric
	const MaltMetric & metric = this->metrics.at(metricName);

	if (inclusive)
		return metric.extractor(dataElement.total);
	else
		return metric.extractor(dataElement.own);
}

/**********************************************************/
std::string MaltFuncMetrics::getFormattedValue(const FlatProfileValue & dataElement, const std::string & metricName, bool inclusive) const
{
	//get value
	size_t value = this->getValue(dataElement, metricName, inclusive);

	//get metric
	const MaltMetric & metric = this->metrics.at(metricName);

	//ret
	return metric.formater(value);
}

}

/*
MaltFuncMetrics.prototype.getValueRatio = function(dataElement,metricName,inclusive)
{
	return (100 *this.getValue(dataElement,metricName,inclusive)) / this.ref[metricName];
}

MaltFuncMetrics.prototype.getRef = function(data,metricName)
{
// 	if (this.refs == undefined)
// 		this.buildRefs(data);
// 	console.log(metricName + " " + this.refs[metricName]);
// 	return this.refs[metricName];
	return this.computeRef(data,metricName);
}

MaltFuncMetrics.prototype.buildRefs = function(data)
{
	this.refs = {};
	for (var i in this.maltMetrics)
		this.refs[i] = this.computeRef(data,this.maltMetrics[i]);
}

MaltFuncMetrics.prototype.computeRef = function(data,metricName)
{
	var metric = this.maltMetrics[metricName];
	var res = 0;
	switch(metric.ref)
	{
		case 'sum':
			for (var i in data)
				res += this.getValue(data[i],metricName);
			break;
		case 'max':
			for (var i in data)
			{
				var tmp = this.getValue(data[i],metricName);
				if (tmp > res)
					res = tmp;
			}
			break;
		default:
			console.log("Invalid value for ref mode for metric "+metricName+" : "+metric.ref);
			res = 1;
	}
	return res;
}

*/
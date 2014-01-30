/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstring>
#include <fstream>
#include <iostream>
#include "ValgrindOutput.hpp"
#include <portability/OS.hpp>

using namespace std;

/********************  MACROS  **********************/
#define LINE_BREAK "\n"

namespace MATT
{

/*******************  FUNCTION  *********************/
void ValgrindOutput::pushStackInfo(SimpleCallStackNode& stackNode)
{
	//get addresses
	Stack & stack = stackNode.getCallStack();
	CallStackInfo & stackInfo = stackNode.getInfo();
	void * leafCalleePtr = stack.getCallee();
	
	//search function info in caller map and reduce data
	ValgrindCaller & funcInfo = callers[leafCalleePtr];
	funcInfo.info.push(stackInfo);
	
	//if as caller/callee, register inclusive costs
	for (int i = 1 ; i < stack.getSize() ; i++)
	{
		//extrace callee/caller
		void * callerPtr = stack[i];
		void * calleePtr = stack[i-1];

		//search info in map
		ValgrindCaller & callerInfo = callers[callerPtr];

		//reduce on caller
		callerInfo.info.push(stackNode.getInfo());

		//cumulate on callee
		CallStackInfo & calleeInfo = callerInfo.callees[calleePtr];
		calleeInfo.push(stackInfo);
	}
}

/*******************  FUNCTION  *********************/
void ValgrindOutput::writeAsCallgrind(const std::string& filename)
{
	ofstream out;
	out.open(filename.c_str());
	writeAsCallgrind(out);
	out.close();
}

/*******************  FUNCTION  *********************/
void ValgrindOutput::writeAsCallgrind(std::ostream& out)
{
	//header
	out << "version: 1" << LINE_BREAK;
	out << "creator: ATT-0.0.0" << LINE_BREAK;
	out << "pid: " << OS::getPID() << LINE_BREAK;
	out << "cmd: " << OS::getExeName() << LINE_BREAK;
	out << "part: 1" << LINE_BREAK;
	out << LINE_BREAK;
	out << "desc: I1 cache: " << LINE_BREAK;
	out << "desc: D1 cache: " << LINE_BREAK;
	out << "desc: LL cache: " << LINE_BREAK;
	out << LINE_BREAK;
	out << "desc: Trigger: Program termination" << LINE_BREAK;
	out << LINE_BREAK;
	out << "positions: line" << LINE_BREAK;
	out << "events: MaxAliveMemory AllocSum Count" << LINE_BREAK;
	out << LINE_BREAK;
	
	//try to extract functions names bases on addresses
	FuncNameDic names;
	
	//loop on data
	for (ValgrindCallerMap::const_iterator it = callers.begin() ; it != callers.end() ; ++it)
	{
		out << "ob=unknown" << LINE_BREAK;
		out << "fn=" << names.getName(it->first) << LINE_BREAK;
		it->second.info.writeAsCallgrindEntry(0,out);
		out << LINE_BREAK;
		for (ValgrindCalleeMap::const_iterator itChild = it->second.callees.begin() ; itChild != it->second.callees.end() ; ++itChild)
		{
			out << "cfn=" << names.getName(itChild->first) << LINE_BREAK;
			out << "calls=1 0" << LINE_BREAK;
			itChild->second.writeAsCallgrindEntry(0,out);
			out << LINE_BREAK;
		}
		out << LINE_BREAK;
	}
}

};

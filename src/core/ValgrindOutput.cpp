/********************  HEADERS  *********************/
#include <cstring>
#include <fstream>
#include "ValgrindOutput.hpp"
//TODO move into postability
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace ATT
{

/*******************  FUNCTION  *********************/
void ValgrindOutput::pushStackInfo(SimpleCallStackNode& stackNode)
{
	//get addresses
	Stack & stack = stackNode.getCallStack();
	CallStackInfo & stackInfo = stackNode.getInfo();
	void * calleePtr = stack.getCallee();
	
	//search function info in caller map and reduce data
	ValgrindCaller & funcInfo = callers[calleePtr];
	funcInfo.info.push(stackInfo);
	
	//if as caller/callee, register inclusive costs
	if (stack.getSize() > 1)
	{
		void * callerPtr = stack.getCaller();

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
	out << "version: 1" << endl;
	out << "creator: ATT-0.0.0" << endl;
	out << "pid: " << getpid() << endl;
	out << "cmd: TODO" << endl;
	out << "part: 1" << endl;
	out << endl;
	out << "desc: I1 cache: " << endl;
	out << "desc: D1 cache: " << endl;
	out << "desc: LL cache: " << endl;
	out << endl;
	out << "desc: Trigger: Program termination" << endl;
	out << endl;
	out << "position: line" << endl;
	out << "event: MaxAliveMemory";
	
	//loop on data
	for (ValgrindCallerMap::const_iterator it = callers.begin() ; it != callers.end() ; ++it)
	{
		out << "ob=unknown" << endl;
		out << "fn=" << it->first << endl;
		it->second.info.writeAsCallgrindEntry(0,out);
		for (ValgrindCallerMap::const_iterator itChild = it->second.callees.begin() ; itChild != it->second.callees.end() ; ++itChild)
		{
			out << "fn=" << itChild->first;
			out << "calls=1 " << itChild->second.info. << endl;
		}
	}
}


};

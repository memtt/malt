/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdlib>

namespace MATT
{

/*********************  CLASS  **********************/
class PerThreadHooks
{
	public:
		PerThreadHooks(void);
		virtual ~PerThreadHooks(void);
		virtual void onInit(void) = 0;
		virtual void onExit(void) = 0;
};
	
/*********************  CLASS  **********************/
class GlobalHooks
{
	public:
		GlobalHooks(void);
		virtual ~GlobalHooks(void);
		virtual void onInit(void) = 0;
		virtual void onExit(void) = 0;
		virtual PerThreadHooks * onThreadStart(void) = 0;
		virtual void onThreadExit(PerThreadHooks * hooks) = 0;
};

}

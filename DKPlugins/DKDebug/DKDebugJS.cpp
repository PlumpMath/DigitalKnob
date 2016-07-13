#ifdef USE_DKDuktape 
#include "DKDebug.h"
#include "DKDebugJS.h"


//////////////////////
void DKDebugJS::Init()
{
	DKDuktape::AttachFunction("DKDebug_SendBugReport", DKDebugJS::SendBugReport, 1);
}

//////////////////////////////////////////////
int DKDebugJS::SendBugReport(duk_context* ctx)
{
	DKString filename = duk_require_string(ctx, 0);
	if(!DKDebug::SendBugReport(filename)){
		return 0;
	}
	return 1;
}


#endif //USE_DKDuktape
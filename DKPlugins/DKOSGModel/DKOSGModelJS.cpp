#ifdef USE_DKDuktape 
#include "DKOSGModel.h"
#include "DKOSGModelJS.h"
#include "DKOSGWindow.h"

//////////////////////
void DKOSGModelJS::Init()
{
	//DKDuktape::AttachFunction("DKOSGModel_NewModel", DKOSGModelJS::NewModel, 1);
}

/*
//////////////////////////////////
int DKOSGModelJS::NewModel(duk_context* ctx)
{
	DKString file = duk_require_string(ctx, 0);
	DKCreate("DKOSGModel,,DKOSGWindow,"+file);
	return 1;
}
*/



#endif //USE_DKDuktape
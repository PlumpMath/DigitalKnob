#include "DKAssetsV8.h"
#include "DKFile.h"
#include "DKApp.h"

/////////////////////
void DKAssetsV8::Init()
{
	DKLog("DKAssetsV8::Init()\n", DKDEBUG);
	//DKCefApp::AttachFunction("Test", DKAssetsV8::Test);
}

///////////////////
void DKAssetsV8::End()
{
	DKLog("DKAssetsV8::End()\n", DKDEBUG);
}

/*
///////////////////////////////////////////////////
bool DKAssetsV8::Test(CefArgs args, CefReturn retval)
{
	DKLog("DKAssetsV8::Test(CefArgs,CefReturn)\n", DKDEBUG);
	DKString data = args[0]->GetStringValue();
	DKString result = data;
	retval = CefV8Value::CreateString(result);
	return true;
}
*/
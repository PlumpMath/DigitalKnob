#include "DKPlugin2.h"

//////////////////////
void DKPlugin2::Init()
{
	DKLog("DKPlugin2::Init()\n", DKINFO);
	DKClass::RegisterFunc("DKPlugin2::SomeFunction", &DKPlugin2::SomeFunction, this);
};

/////////////////////////////////////////
void* DKPlugin2::SomeFunction(void* data)
{
	DKLog("DKPlugin2::SomeFunction()\n", DKINFO);
	return NULL;
};


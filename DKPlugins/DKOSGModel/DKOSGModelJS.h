#ifdef USE_DKDuktape 
#pragma once
#ifndef DKOSGModelJS_H
#define DKOSGModelJS_H

#include "DKDuktape.h"

/////////////////////////////////////////////
class DKOSGModelJS : public DKObjectT<DKOSGModelJS>
{
public:
	void Init();
	//static int NewModel(duk_context* ctx);
};

REGISTER_OBJECT(DKOSGModelJS, true)

#endif //DKOSGModelJS_H
#endif //USE_DKDuktape
#pragma once
#ifndef DKWebview_H
#define DKWebview_H

#ifdef ANDROID
#include <jni.h>
#endif

/////////////////////////////////////////////
class DKWebview : public DKObjectT<DKWebview>
{
public:
	void Init();
	void End();
	
	bool onCreate(void* input, void* output);
	bool Test(void* input, void* output);
	bool SendValue(void* input, void* output);
	bool ReceiveValue(void* input, void* output);
	bool PrintFunctions(void* input, void* output);
	
};

REGISTER_OBJECT(DKWebview, true)

#endif //DKWebview_H


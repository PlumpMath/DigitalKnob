#ifdef USE_DKDuktape 
#pragma once
#ifndef DKCurlJS_H
#define DKCurlJS_H

#include "DKDuktape.h"

///////////////////////////////////////////
class DKCurlJS : public DKObjectT<DKCurlJS>
{
public:
	void Init();
	static int FtpConnect(duk_context* ctx);
	static int FtpUpload(duk_context* ctx);
	static int HttpToString(duk_context* ctx);
	static int HttpFileExists(duk_context* ctx);
	static int FileDate(duk_context* ctx);
	static int Download(duk_context* ctx);
};

REGISTER_OBJECT(DKCurlJS, true)

#endif //DKCurlJS_H
#endif //USE_DKDuktape
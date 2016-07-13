#ifdef USE_DKDuktape 
#pragma once
#ifndef DKWidgetMySqlJS_H
#define DKWidgetMySqlJS_H

#include "DKDuktape.h"
#include "DKWidget.h"

/////////////////////////////////////////////////////////
class DKWidgetMySqlJS : public DKObjectT<DKWidgetMySqlJS>
{
public:
	void Init();
	static int Prep(duk_context* ctx);
	static int PrepField(const DKString& database, const DKString& table, const DKString& field);
	static int GetFirstRecordNum(duk_context* ctx);
	static int GetLastRecordNum(duk_context* ctx);
	static int GetPrevRecordNum(duk_context* ctx);
	static int GetNextRecordNum(duk_context* ctx);
	static int LoadRecord(duk_context* ctx);
	static int SaveRecord(duk_context* ctx);
	static int DeleteRecord(duk_context* ctx);
	static int Search(duk_context* ctx);
};


REGISTER_OBJECT(DKWidgetMySqlJS, true)

#endif //DKWidgetMySqlJS_H
#endif //USE_DKDuktape
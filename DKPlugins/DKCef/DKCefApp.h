#pragma once
#ifndef DKCefApp_H
#define DKCefApp_H
#include <include/cef_app.h>

typedef CefV8ValueList CefArgs;
typedef CefRefPtr<CefV8Value>& CefReturn;
class DKCefApp;

//#ifndef MAC
///////////////////////////////////////
class MyV8Handler : public CefV8Handler
{
public:
	MyV8Handler()
	{
		DKLog("MyV8Handler::MyV8Handler()\n",DKDEBUG);
	}

#ifdef MAC
	static std::map<DKString, boost::function2<bool, CefArgs, CefReturn> > functions;
#else
	static std::map<DKString, boost::function<bool (CefArgs, CefReturn)>> functions;
#endif
	

	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefArgs& arguments, 
						CefReturn retval, CefString& exception) OVERRIDE {
		DKLog("MyV8Handler::Execute()\n", DKDEBUG);
		if(!functions[name]) {
			DKLog("MyV8Handler::Execute("+DKString(name)+") not registered\n", DKWARN);
			return false;
		}
		if(!functions[name](arguments, retval)){
			DKLog("MyV8Handler::Execute("+DKString(name)+") failed\n", DKERROR);
			return false;
		}
		return true;
	}

	IMPLEMENT_REFCOUNTING(MyV8Handler);
};
//#endif //WIN32

///////////////////////////////////////////////////////////////////////////////////////////////
class DKCefApp : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler
{
public:
	DKCefApp(){}

//#ifndef MAC
	static CefRefPtr<MyV8Handler> handler;
//#endif
	static CefRefPtr<CefV8Value> object;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE { return this; }

	virtual void OnContextInitialized();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
	{
		DKLog("DKCefApp::OnBeforeCommandLineProcessing()\n", DKDEBUG);
		
		command_line->AppendSwitchWithValue("enable-system-flash", "1");
		command_line->AppendSwitchWithValue("allow-file-access-from-files", "1");
		command_line->AppendSwitchWithValue("disable-gpu", "1");
		command_line->AppendSwitchWithValue("disable-gpu-compositing", "1");
		command_line->AppendSwitchWithValue("ignore-gpu-blacklist", "1");
		command_line->AppendSwitchWithValue("remote-debugging-port", "2393");
		command_line->AppendSwitchWithValue("disable-web-security", "1");
		command_line->AppendSwitchWithValue("no-proxy-server", "1");
		command_line->AppendSwitchWithValue("enable-webgl", "1");
		//command_line->AppendSwitchWithValue("no-sandbox", "1");
		//command_line->AppendSwitchWithValue("renderer-process-limit", "1");
		//command_line->AppendSwitchWithValue("enable-begin-frame-scheduling", "1"); //Breaks Popups
//#ifndef MAC	
		handler = new MyV8Handler();
//#endif		
		DKCreate("DKCefV8");
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE
	{
		DKLog("DKCefApp::OnContextCreated()\n", DKDEBUG);	
		object = context->GetGlobal(); // Retrieve the context's window object.
		AttachFunctions();
	}

	//////////////////////////////
	static void AttachFunctions()
	{
		if(!object){
			DKLog("DKCefApp::AttachFunctions(): DKCefApp::OnContextCreated() has not been called yet. \n", DKERROR);
			return;
		}

#ifdef MAC
		typedef std::map<DKString, boost::function2<bool, CefArgs, CefReturn> >::iterator it_type;
#else
		typedef std::map<DKString, boost::function<bool (CefArgs, CefReturn)>>::iterator it_type;
#endif
		for(it_type iterator = handler->functions.begin(); iterator != handler->functions.end(); iterator++) {
			CefRefPtr<CefV8Value> value = CefV8Value::CreateFunction(iterator->first.c_str(), handler);
			object->SetValue(iterator->first.c_str(), value, V8_PROPERTY_ATTRIBUTE_NONE);
		}
//#endif		
	}

	//////////////////////////////////////////////////////////////////////////////////
	static void AttachFunction(const DKString& name, bool (*func)(CefArgs, CefReturn))
	{
//#ifndef MAC
		//NOTE: stoes the function, it will be attached when OnContextCreated is called.
		DKLog("DKCefApp::AttachFunction("+name+")\n", DKDEBUG);
		handler->functions[name] = boost::bind(func, _1, _2);
		if(object){
			CefRefPtr<CefV8Value> value = CefV8Value::CreateFunction(name.c_str(), handler);
			object->SetValue(name.c_str(), value, V8_PROPERTY_ATTRIBUTE_NONE);
		}
		if(!handler->functions[name]){
			DKLog("DKCefApp::AttachFunctions()("+name+"): failed to register function \n", DKERROR);
			return;
		}
//#endif		
	}

	IMPLEMENT_REFCOUNTING(DKCefApp);
};



#endif //DKCefApp_H
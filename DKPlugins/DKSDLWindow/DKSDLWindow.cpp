#include "stdafx.h"
#include "DKSDLWindow.h"
#include "DKFile.h"
#include "SDL_syswm.h"
#include "DKAndroid.h"

#if defined(WIN32) || defined(MAC) || defined(LINUX)
	#include <GL/gl.h>
#endif
/*
#if defined(ANDROID)
	#include <GLES/gl.h>
#endif
#if defined(IOS)
	#include <OpenGLES/ES1/gl.h>
#endif
*/


std::vector<boost::function<bool(SDL_Event *event)> > DKSDLWindow::event_funcs;
std::vector<boost::function<void()> > DKSDLWindow::draw_funcs;
std::map<int,int> DKSDLWindow::sdlKeyCode;
std::map<int,int> DKSDLWindow::sdlCharCode;
std::map<int,int> DKSDLWindow::sdlShiftCharCode;
std::map<int,int> DKSDLWindow::sdlMacCode;

////////////////////////
void DKSDLWindow::Init()
{
	DKLog("DKSDLWindow::Init()\n", DKINFO);

#ifdef ANDROID
		//DKLog("CallJavaFunction(OpenActivity,SDLActivity)\n", DKDEBUG);
		//CallJavaFunction("OpenActivity","SDLActivity");
#endif

	//Get values from settings.txt file
	DKString sdl_renderer;
	DKFile::GetSetting(DKFile::local_assets+"settings.txt", "[SDL_RENDERER]", sdl_renderer);
	DKLog("settings.txt: [SDL_RENDERER] = "+sdl_renderer+"\n", DKINFO);
	
	SDL_SetMainReady();
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0){
		DKLog("SDL_Init Error: "+DKString(SDL_GetError()), DKERROR);
		return;
	}

	DKString title;
	DKFile::GetExeName(title);
	DKFile::RemoveExtention(title);

	mTitle = title;
	winX = 0;
	winY = 0;
	width = 0;
	height = 0;
	last_mouseX = 0;
	last_mouseY = 0;

	DKString textX;
	DKFile::GetSetting(DKFile::local_assets+"settings.txt", "[WINX]", textX);
	DKString textY;
	DKFile::GetSetting(DKFile::local_assets+"settings.txt", "[WINY]", textY);
	DKString textWidth;
	DKFile::GetSetting(DKFile::local_assets+"settings.txt", "[WIDTH]", textWidth);
	DKString textHeight;
	DKFile::GetSetting(DKFile::local_assets+"settings.txt", "[HEIGHT]", textHeight);
	if(!textX.empty()){ winX = toInt(textX); }
	if(!textY.empty()){ winY = toInt(textY); }
	if(!textWidth.empty()){ width = toInt(textWidth); }
	if(!textHeight.empty()){ height = toInt(textHeight); }

	#ifdef WIN32 //account window frame
		winY = (winY + 30); 
		winX = (winX + 10);
	#endif
	if(winX < 1){winX = 20;}
	if(winY < 1){winY = 40;}
	if(width < 1){ width = 800; }
	if(height < 1){height = 600;}
    
#ifdef IOS
    width = 320;
    height = 480;
#endif
#ifdef ANDROID
    width = DKAndroid::android_width;
	height = DKAndroid::android_height;
#endif

	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
#ifndef ANDROID
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif
	
	DKString result;
	
#if defined(ANDROID) || defined(IOS)
	DKLog("Creating SDLWindow for mobile device  \n", DKINFO);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	result = "OpenglES";
	DKLog("DKSDLWindow Width: "+toString(width)+" Height: "+toString(height)+"\n", DKINFO);
	if(SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &sdlwin, &sdlren) < 0){
		DKLog("SDL_CreateWindow Error: "+DKString(SDL_GetError()), DKERROR);
		return;
	}
#endif

#if !defined(ANDROID) && !defined(IOS)
	DKLog("Creating SDLWindow for Desktop \n", DKINFO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	sdlwin = SDL_CreateWindow(mTitle.c_str(), winX, winY, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if(!sdlwin){
		DKLog("SDL_CreateWindow Error: "+DKString(SDL_GetError()), DKERROR);
		SDL_Quit();
		return;
	}

	sdlren = NULL;
	if(!same(sdl_renderer, "SOFTWARE")){
		result = "SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC";
		sdlren = SDL_CreateRenderer(sdlwin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}
	if(!sdlren){
		result = "SDL_RENDERER_SOFTWARE";
		sdlren = SDL_CreateRenderer(sdlwin, -1, SDL_RENDERER_SOFTWARE);	
	}
	if(!sdlren){
		SDL_DestroyWindow(sdlwin);
		DKLog("SDL_CreateRenderer Error: "+DKString(SDL_GetError()), DKERROR);
		SDL_Quit();
		return;
	}
#endif

	//Set window Title
	DKString title2; 
	DKFile::GetExeName(title2);
#if defined(WIN32) && !defined(WIN64)
		title2 += " - WIN32";
#endif
#ifdef WIN64
		title2 += " - WIN64";
#endif
#ifdef MAC
		title2 += " - MAC";
#endif
#ifdef LINUX
		title2 += " - LINUX";
#endif
#ifdef DEBUG
		title2 += " DEBUG ";
#else
		title2 += " RELEASE ";
#endif
	DKString mTime;
	DKString file;
	DKFile::GetFullExeName(file);
	DKFile::GetModifiedTime(file, mTime);
	title2 += mTime;
	
	SetIcon(DKFile::local_assets+"icon.ico");

	//DKLog(title+"\n", DKINFO);
	SDL_SetWindowTitle(sdlwin, title2.c_str());

	DKClass::RegisterFunc("DKSDLWindow::TestInt", &DKSDLWindow::TestInt, this);
	DKClass::RegisterFunc("DKSDLWindow::TestString", &DKSDLWindow::TestString, this);
	DKClass::RegisterFunc("DKSDLWindow::TestReturnInt", &DKSDLWindow::TestReturnInt, this);
	DKClass::RegisterFunc("DKSDLWindow::TestReturnString", &DKSDLWindow::TestReturnString, this);

	DKClass::RegisterFunc("DKSDLWindow::Fullscreen", &DKSDLWindow::Fullscreen, this);
	DKClass::RegisterFunc("DKSDLWindow::GetClipboard", &DKSDLWindow::GetClipboard, this);
	DKClass::RegisterFunc("DKSDLWindow::GetHeight", &DKSDLWindow::GetHeight, this);
	DKClass::RegisterFunc("DKSDLWindow::GetHwnd", &DKSDLWindow::GetHwnd, this);
	DKClass::RegisterFunc("DKSDLWindow::GetMouseX", &DKSDLWindow::GetMouseX, this);
	DKClass::RegisterFunc("DKSDLWindow::GetMouseY", &DKSDLWindow::GetMouseY, this);
	DKClass::RegisterFunc("DKSDLWindow::GetScreenHeight", &DKSDLWindow::GetScreenHeight, this);
	DKClass::RegisterFunc("DKSDLWindow::GetScreenWidth", &DKSDLWindow::GetScreenWidth, this);
	DKClass::RegisterFunc("DKSDLWindow::GetWidth", &DKSDLWindow::GetWidth, this);
	DKClass::RegisterFunc("DKSDLWindow::GetX", &DKSDLWindow::GetX, this);
	DKClass::RegisterFunc("DKSDLWindow::GetY", &DKSDLWindow::GetY, this);
	DKClass::RegisterFunc("DKSDLWindow::Hide", &DKSDLWindow::Hide, this);
	DKClass::RegisterFunc("DKSDLWindow::IsFullscreen", &DKSDLWindow::IsFullscreen, this);
	DKClass::RegisterFunc("DKSDLWindow::IsVisible", &DKSDLWindow::IsVisible, this);
	DKClass::RegisterFunc("DKSDLWindow::Minimize", &DKSDLWindow::Minimize, this);
	DKClass::RegisterFunc("DKSDLWindow::Restore", &DKSDLWindow::Restore, this);
	DKClass::RegisterFunc("DKSDLWindow::SetClipboard", &DKSDLWindow::SetClipboard, this);
	DKClass::RegisterFunc("DKSDLWindow::SetHeight", &DKSDLWindow::SetHeight, this);
	DKClass::RegisterFunc("DKSDLWindow::SetWidth", &DKSDLWindow::SetWidth, this);
	DKClass::RegisterFunc("DKSDLWindow::SetX", &DKSDLWindow::SetX, this);
	DKClass::RegisterFunc("DKSDLWindow::SetY", &DKSDLWindow::SetY, this);
	DKClass::RegisterFunc("DKSDLWindow::Show", &DKSDLWindow::Show, this);
	DKClass::RegisterFunc("DKSDLWindow::Windowed", &DKSDLWindow::Windowed, this);
	DKClass::RegisterFunc("DKSDLWindow::MessageBox", &DKSDLWindow::MessageBox, this);
	DKApp::AppendLoopFunc(&DKSDLWindow::Process, this);
	DKSDLWindow::AddEventFunc(&DKSDLWindow::handle, this);

	MapInputs();
	SDL_SetEventFilter(&DKSDLWindow::EventFilter, this);

#if !defined(ANDROID) && !defined(IOS)
	SDL_GLContext glcontext = SDL_GL_CreateContext(sdlwin);
	SDL_GL_MakeCurrent(sdlwin, glcontext);
	gl_version = (char*)glGetString(GL_VERSION);
	// these are only available with OpenGL 3.0+
	// https://www.khronos.org/opengl/wiki/Get_Context_Info
	// gl_major_version = (char*)glGetString(GL_MAJOR_VERSION);
	// gl_minor_version = (char*)glGetString(GL_MINOR_VERSION);
	gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	gl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	//gl_shading = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	gl_extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

	DKLog("##############################\n", DKINFO);
	DKLog("##### Window Information #####\n", DKINFO);
	DKLog("##############################\n", DKINFO);
	DKLog("GL_MAJOR_VERSION = "+gl_major_version+"\n", DKINFO);
	DKLog("GL_MINOR_VERSION = "+gl_minor_version+"\n", DKINFO);
	DKLog("GL_VERSION = "+gl_version+"\n", DKINFO);
	DKLog("GL_VENDOR = "+gl_vendor+"\n", DKINFO);
	DKLog("GL_RENDERER = "+gl_renderer+"\n", DKINFO);
	//DKLog("GL_SHADING_LANGUAGE_VERSION = "+gl_shading+"\n", DKINFO);
	//DKLog("GL_EXTENSIONS = "+gl_extensions+"\n", DKINFO);
	DKLog("SDL Renderer = "+result+"\n", DKINFO);
	DKLog("Resolution = "+toString(width)+"x"+toString(height)+"\n", DKINFO);
	int depth;
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth);
	DKLog("Depth = "+toString(depth)+"\n", DKINFO);
	int doublebuffer;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doublebuffer);
	DKLog("Double buffer = "+toString(doublebuffer)+"\n", DKINFO);
	int red;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red);
	DKLog("Red Size = "+toString(red)+"\n", DKINFO);
	int green;
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green);
	DKLog("Green Size = "+toString(green)+"\n", DKINFO);
	int blue;
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue);
	DKLog("Blue Size = "+toString(blue)+"\n", DKINFO);
	int alpha;
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha);
	DKLog("Alpha Size = "+toString(alpha)+"\n", DKINFO);
	int buffer;
	SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &buffer);
	DKLog("Buffer Size = "+toString(buffer)+"\n", DKINFO);
	int stencil;
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
	DKLog("Stencil Size = "+toString(stencil)+"\n", DKINFO);
	
	if(has(gl_vendor, "Microsoft")){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenGL Drivers", "Your OpenGL video drivers are old and out of date. Please upgrade the graphics card drivers for best performance and compatability.", sdlwin);
		DKApp::Exit();
	}
#endif
}

///////////////////////
void DKSDLWindow::End()
{
	//SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(sdlren);
	SDL_DestroyWindow(sdlwin);
	SDL_Quit();
}

///////////////////////////////////////////////
bool DKSDLWindow::SetIcon(const DKString& file)
{
#ifdef WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdlwin, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	HICON hIcon = (HICON)LoadImage(NULL, file.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	return true;
#endif
	DKLog("DKSDLWindow::SetIcon is not implemented on this OS. \n", DKERROR);
	return false;
}




////////////////////////////////////////////////////
bool DKSDLWindow::TestInt(void* input, void* output)
{
	int in = *(int*)input;
	int out = in;
	*(int*)output = out;
	return true;
}

///////////////////////////////////////////////////////
bool DKSDLWindow::TestString(void* input, void* output)
{
	std::string in = *(std::string*)input;
	std::string out = in;
	*(std::string*)output = out;
	return true;
}

//////////////////////////////////////////////////////////
bool DKSDLWindow::TestReturnInt(void* input, void* output)
{
	int var = 1234;
	*(int*)output = var;
	return true;
}

/////////////////////////////////////////////////////////////
bool DKSDLWindow::TestReturnString(void* input, void* output)
{
	std::string var = "Return test";
	*(std::string*)output = var;
	return true;
}

//////////////////////////////////
bool DKSDLWindow::GetClipboard(void* input, void* output)
{
	//TODO
	std::string out = SDL_GetClipboardText();
	*(std::string*)output = out;
	return true;
}

//////////////////////////////
bool DKSDLWindow::GetX(void* input, void* output)
{
	int x;
	SDL_GetWindowPosition(sdlwin, &x, NULL);
	*(int*)output = x;
	return true;
}

//////////////////////////////
bool DKSDLWindow::GetY(void* input, void* output)
{
	int y;
	SDL_GetWindowPosition(sdlwin, NULL, &y);
	*(int*)output = y;
	return true;
}

//////////////////////////////////
bool DKSDLWindow::GetWidth(void* input, void* output)
{
	int w;
	SDL_GetWindowSize(sdlwin, &w, NULL);
	*(int*)output = w;
	return true;
}

///////////////////////////////////
bool DKSDLWindow::GetHeight(void* input, void* output)
{
	int h;
	SDL_GetWindowSize(sdlwin, NULL, &h);
	*(int*)output = h;
	return true;
}

//////////////////////////////////
bool DKSDLWindow::SetClipboard(void* input, void* output)
{
	DKLog("DKSDLWindow::SetClipboard()\n", DKINFO);
	std::string in = *(std::string*)input;
	if(in.empty()){ return false; }
	if(SDL_SetClipboardText(in.c_str()) < 0){ return false; }
	return true;	
}

////////////////////////////////
bool DKSDLWindow::SetX(void* input, void* output)
{
	int x = *(int*)input;
	int y;
	SDL_GetWindowPosition(sdlwin, NULL, &y);
	SDL_SetWindowPosition(sdlwin, x, y);
	return true;
}

////////////////////////////////
bool DKSDLWindow::SetY(void* input, void* output)
{
	int y = *(int*)input;
	int x;
	SDL_GetWindowPosition(sdlwin, &x, NULL);
	SDL_SetWindowPosition(sdlwin, x, y);
	return true;
}

////////////////////////////////////
bool DKSDLWindow::SetWidth(void* input, void* output)
{
	int w = *(int*)input;
	int h;
	SDL_GetWindowSize(sdlwin, NULL, &h);
	SDL_SetWindowSize(sdlwin, w, h);
	return true;
}

/////////////////////////////////////
bool DKSDLWindow::SetHeight(void* input, void* output)
{
	int h = *(int*)input;
	int w;
	SDL_GetWindowSize(sdlwin, &w, NULL);
	SDL_SetWindowSize(sdlwin, w, h);
	return true;
}

////////////////////////////////////////
bool DKSDLWindow::GetScreenWidth(void* input, void* output)
{
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	*(int*)output = dm.w;
	return true;
}

/////////////////////////////////////////
bool DKSDLWindow::GetScreenHeight(void* input, void* output)
{
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	*(int*)output = dm.h;
	return true;
}

//////////////////////////////////////
bool DKSDLWindow::IsFullscreen(void* input, void* output)
{
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool isFullscreen = SDL_GetWindowFlags(sdlwin) & FullscreenFlag;
	*(bool*)output = isFullscreen;
	return true;
}

////////////////////////////////////
bool DKSDLWindow::Fullscreen(void* input, void* output)
{
	SDL_SetWindowFullscreen(sdlwin, SDL_WINDOW_FULLSCREEN_DESKTOP);
	return true;
}

//////////////////////////////////
bool DKSDLWindow::Windowed(void* input, void* output)
{
	SDL_SetWindowFullscreen(sdlwin, 0); 
	return true;
}

/////////////////////////////////
bool DKSDLWindow::Restore(void* input, void* output)
{
	SDL_RestoreWindow(sdlwin);
	return true;
}

//////////////////////////////////
bool DKSDLWindow::Minimize(void* input, void* output)
{
	SDL_MinimizeWindow(sdlwin);
	return true;
}

///////////////////////////////////
bool DKSDLWindow::IsVisible(void* input, void* output)
{
	Uint32 IsVisibleFlag = SDL_WINDOW_SHOWN;
    bool isVisible = SDL_GetWindowFlags(sdlwin) & IsVisibleFlag;
	*(bool*)output = isVisible;
	return true;
}

//////////////////////////////
bool DKSDLWindow::Hide(void* input, void* output)
{
	SDL_HideWindow(sdlwin);
	return true;
}

//////////////////////////////
bool DKSDLWindow::Show(void* input, void* output)
{
	SDL_ShowWindow(sdlwin);
	return true;
}

///////////////////////////////////
bool DKSDLWindow::GetMouseX(void* input, void* output)
{
	int mouseX;
	SDL_GetMouseState(&mouseX, NULL);
	*(int*)output = mouseX;
	return true;
}

///////////////////////////////////
bool DKSDLWindow::GetMouseY(void* input, void* output)
{
	int mouseY;
	SDL_GetMouseState(NULL, &mouseY);
	*(int*)output = mouseY;
	return true;
}

/////////////////////////////////
bool DKSDLWindow::GetHwnd(void* input, void* output)
{
#ifdef WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdlwin, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	*(HWND*)output = hwnd;
	return true;
#else
	DKLog("DKSDLWindow::GetHwnd(): This OS does not have a HWND handle \n", DKERROR);
	return false;
#endif
}

////////////////////////////////////////////////////////
bool DKSDLWindow::MessageBox(void* input, void* output)
{
	DKString message = *(DKString*)input;
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "DKERROR", message.c_str(), sdlwin);
	return true;
}

///////////////////////////
void DKSDLWindow::Process()
{
	if(SDL_GetWindowFlags(sdlwin) & SDL_WINDOW_HIDDEN){ 
			DKUtil::Sleep(1000); //FIXME - look for a better way to save cpu usage here	
	}
	
	if(SDL_GetWindowFlags(sdlwin) & SDL_WINDOW_SHOWN){ 
		SDL_SetRenderTarget(sdlren, NULL); 
		SDL_SetRenderDrawColor(sdlren, 178, 178, 220, 255); //light grey w/ blue tint
		//SDL_SetRenderDrawColor(sdlren, 255, 255, 255, 255); //white
		SDL_RenderClear(sdlren);
		SDL_SetRenderDrawColor(sdlren, 0, 0, 0, 255); //black
		SDL_RenderDrawLine(sdlren, 0, height / 2, width, height / 2 );
	}

	SDL_Event e;
	while(SDL_PollEvent(&e)){
		for(unsigned int i = 0; i < event_funcs.size(); ++i){
			if(event_funcs[i](&e)){ //Call event functions
				i = event_funcs.size();	//eat the event
			}; 
		}
	}
	
	if(SDL_GetWindowFlags(sdlwin) & SDL_WINDOW_SHOWN){ 
		for(unsigned int i = 0; i < draw_funcs.size(); ++i){
			draw_funcs[i](); //Call event functions
		}

		SDL_RenderPresent(sdlren);
	}
}

//////////////////////////////////////////////////////////////
int DKSDLWindow::EventFilter(void* userdata, SDL_Event* event)
{
	if(event->type == SDL_WINDOWEVENT){
		switch(event->window.event){
			case SDL_WINDOWEVENT_MOVED:{
				DKSDLWindow* dksdlwindow = static_cast<DKSDLWindow*>(userdata);
				dksdlwindow->winX = event->window.data1;
				dksdlwindow->winY = event->window.data2;
				dksdlwindow->Process();
				DKEvent::SendEvent("GLOBAL", "move", toString(dksdlwindow->winX)+","+toString(dksdlwindow->winY));
				return 1;
			}
			case SDL_WINDOWEVENT_RESIZED:{
				DKSDLWindow* dksdlwindow = static_cast<DKSDLWindow*>(userdata);
				dksdlwindow->width = event->window.data1;
				dksdlwindow->height = event->window.data2;
				dksdlwindow->Process();
				DKEvent::SendEvent("GLOBAL", "resize", toString(dksdlwindow->width)+","+toString(dksdlwindow->height));
				return 1;
			}
			case SDL_WINDOWEVENT_SIZE_CHANGED:{
				DKSDLWindow* dksdlwindow = static_cast<DKSDLWindow*>(userdata);
				dksdlwindow->width = event->window.data1;
				dksdlwindow->height = event->window.data2;
				dksdlwindow->Process();
				DKEvent::SendEvent("GLOBAL", "resize", toString(dksdlwindow->width)+","+toString(dksdlwindow->height));
				return 1;
			}
			case SDL_WINDOWEVENT_MINIMIZED:{
				DKEvent::SendEvent("GLOBAL", "minimize", "");
				return 1;
			}
			case SDL_WINDOWEVENT_MAXIMIZED:{
				DKEvent::SendEvent("GLOBAL", "maximize", "");
				return 1;
			}
			case SDL_WINDOWEVENT_RESTORED:{
				DKEvent::SendEvent("GLOBAL", "restore", "");
				return 1;
			}							   
		}
	}
	if(event->type == SDL_APP_WILLENTERBACKGROUND){
		DKLog("DKSDLWindow::SDL_APP_WILLENTERBACKGROUND \n", DKDEBUG);
		DKApp::paused = true;
		return 1;
	}
	if(event->type == SDL_APP_DIDENTERFOREGROUND){
		DKLog("DKSDLWindow::SDL_APP_DIDENTERFOREGROUND \n", DKDEBUG);
		DKApp::paused = false;
		return 1;
	}
	if(event->type == SDL_TEXTINPUT){
		//DKLog("DKSDLWindow::SDL_TEXTINPUT \n", DKDEBUG);
		return 1;
	}
	return 1;
}

//////////////////////////////////////////
bool DKSDLWindow::handle(SDL_Event *event)
{
	switch(event->type){
		case SDL_QUIT:{
			DKApp::Exit();
			return false; //allow event to continue
		}

		case SDL_MOUSEMOTION:{
			if(event->motion.x != last_mouseX || event->motion.y != last_mouseY){
				last_mouseX = event->motion.x;
				last_mouseY = event->motion.y;
				DKEvent::SendEvent("GLOBAL", "mousemove", toString(last_mouseX)+","+toString(last_mouseY)+","+toString(winX+last_mouseX)+","+toString(winY+last_mouseY));
			}
            return false; //allow event to continue
		}
                
		case SDL_MOUSEBUTTONDOWN:{
			DKEvent::SendEvent("GLOBAL", "mousedown", toString(event->button.button));
            return false; //allow event to continue
		}

        case SDL_MOUSEBUTTONUP:{
			DKEvent::SendEvent("GLOBAL", "mouseup", toString(event->button.button));
			if(event->button.button == 3){
				DKEvent::SendEvent("GLOBAL", "contextmenu", toString(event->button.button));
			}
			else{
				if(event->button.clicks == 2){
					DKEvent::SendEvent("GLOBAL", "dblclick", toString(event->button.button));
				}
				else{
					DKEvent::SendEvent("GLOBAL", "click", toString(event->button.button));
				}
			}
            return false; //allow event to continue
		}

        case SDL_MOUSEWHEEL:{
			DKEvent::SendEvent("GLOBAL", "wheel", toString(event->wheel.y));
            return false; //allow event to continue
		}

        case SDL_KEYDOWN:{
			//DKLog("DKSDLWindow::SDL_KEYDOWN("+toString(event->key.keysym.sym)+")\n", DKDEBUG);
			if(event->key.keysym.sym == 0){ return true; }
			if(event->key.keysym.sym > 96 && event->key.keysym.sym < 123){ //letter
				if(event->key.keysym.mod & KMOD_SHIFT && event->key.keysym.mod & KMOD_CAPS){ //both = lowercase
					DKEvent::SendEvent("GLOBAL", "keypress", toString(sdlCharCode[event->key.keysym.sym]));
				}
				else if(event->key.keysym.mod & KMOD_SHIFT || event->key.keysym.mod & KMOD_CAPS){ //1 = uppercase
					DKEvent::SendEvent("GLOBAL", "keypress", toString(sdlShiftCharCode[event->key.keysym.sym]));
				}
				else{
					DKEvent::SendEvent("GLOBAL", "keypress", toString(sdlCharCode[event->key.keysym.sym])); //lowercase
				}
			}
			else if(event->key.keysym.mod & KMOD_SHIFT){ //other character keys
				DKEvent::SendEvent("GLOBAL", "keypress", toString(sdlShiftCharCode[event->key.keysym.sym])); //shifted symbol
			}
			else{
				DKEvent::SendEvent("GLOBAL", "keypress", toString(sdlCharCode[event->key.keysym.sym])); //symbol
			}
			DKEvent::SendEvent("GLOBAL", "keydown", toString(sdlKeyCode[event->key.keysym.sym])); //keycode
			return false; //allow event to continue
		}
		
		case SDL_KEYUP:{
			if(event->key.keysym.sym == 0){ return true; }
			DKEvent::SendEvent("GLOBAL", "keyup", toString(sdlKeyCode[event->key.keysym.sym]));
            return false; //allow event to continue
		}
	}
	return false; //allow event to continue
}

/////////////////////////////
void DKSDLWindow::MapInputs()
{
	//sdlKeyCode translates (SDL_Event->key.keysym.sym) to keycode
	sdlKeyCode[27] = 27; //esc
	sdlKeyCode[1073741882] = 112; //f1
	sdlKeyCode[1073741883] = 113; //f2
	sdlKeyCode[1073741884] = 114; //f3
	sdlKeyCode[1073741885] = 115; //f4
	sdlKeyCode[1073741886] = 116; //f5
	sdlKeyCode[1073741887] = 117; //f6
	sdlKeyCode[1073741888] = 118; //f7
	sdlKeyCode[1073741889] = 119; //f8
	sdlKeyCode[1073741890] = 120; //f9
	sdlKeyCode[1073741891] = 121; //f10
	sdlKeyCode[1073741892] = 122; //f11
	sdlKeyCode[1073741893] = 123; //f12
	sdlKeyCode[1073741894] = 44; //printscreen
	sdlKeyCode[1073741895] = 145; //scrolllock
	sdlKeyCode[1073741896] = 19; //pause
	sdlKeyCode[96] = 192; //tilde
	sdlKeyCode[48] = 48; //0
	sdlKeyCode[49] = 49; //1
	sdlKeyCode[50] = 50; //2
	sdlKeyCode[51] = 51; //3
	sdlKeyCode[52] = 52; //4
	sdlKeyCode[53] = 53; //5
	sdlKeyCode[54] = 54; //6
	sdlKeyCode[55] = 55; //7
	sdlKeyCode[56] = 56; //8
	sdlKeyCode[57] = 57; //9
	sdlKeyCode[45] = 189; //minus
	sdlKeyCode[61] = 187; //equal
	sdlKeyCode[92] = 220; //backslash
	sdlKeyCode[8] = 8; //backspace
	sdlKeyCode[1073741901] = 35; //end
	sdlKeyCode[1073741898] = 36; //home
	sdlKeyCode[1073741907] = 144; //numlock
	sdlKeyCode[1073741908] = 111; //kp_slash
	sdlKeyCode[1073741909] = 106; //kp_multiply
	sdlKeyCode[1073741910] = 109; //kp_minus
	sdlKeyCode[9] = 9; //tab
	sdlKeyCode[113] = 81; //q
	sdlKeyCode[119] = 87; //w
	sdlKeyCode[101] = 69; //e
	sdlKeyCode[114] = 82; //r
	sdlKeyCode[116] = 84; //t
	sdlKeyCode[121] = 89; //y
	sdlKeyCode[117] = 85; //u
	sdlKeyCode[105] = 73; //i
	sdlKeyCode[111] = 79; //o
	sdlKeyCode[112] = 80; //p
	sdlKeyCode[91] = 219; //leftbracket
	sdlKeyCode[93] = 221; //rightbracket
	sdlKeyCode[13] = 13; //enter
	sdlKeyCode[127] = 46; //delete
	sdlKeyCode[1073741897] = 45; //insert
	sdlKeyCode[1073741899] = 33; //pageup
	sdlKeyCode[1073741902] = 34; //pagedown
	sdlKeyCode[1073741922] = 96; //kp_0
	sdlKeyCode[1073741913] = 97; //kp_1
	sdlKeyCode[1073741914] = 98; //kp_2
	sdlKeyCode[1073741915] = 99; //kp_3
	sdlKeyCode[1073741916] = 100; //kp_4
	sdlKeyCode[1073741917] = 101; //kp_5
	sdlKeyCode[1073741918] = 102; //kp_6
	sdlKeyCode[1073741919] = 103; //kp_7
	sdlKeyCode[1073741920] = 104; //kp_8
	sdlKeyCode[1073741921] = 105; //kp_9
	sdlKeyCode[1073741923] = 110; //kp_period
	sdlKeyCode[1073741911] = 107; //kp_plus
	sdlKeyCode[1073741912] = 13; //kp_enter
	sdlKeyCode[1073741881] = 20; //capslock
	sdlKeyCode[97] = 65; //a
	sdlKeyCode[115] = 83; //s
	sdlKeyCode[100] = 68; //d
	sdlKeyCode[102] = 70; //f
	sdlKeyCode[103] = 71; //g
	sdlKeyCode[104] = 72; //h
	sdlKeyCode[106] = 74; //j
	sdlKeyCode[107] = 75; //k
	sdlKeyCode[108] = 76; //l
	sdlKeyCode[59] = 186; //semicolon
	sdlKeyCode[39] = 222; //quote
	sdlKeyCode[1073742049] = 16; //leftshift
	sdlKeyCode[122] = 90; //z
	sdlKeyCode[120] = 88; //x
	sdlKeyCode[99] = 67; //c
	sdlKeyCode[118] = 86; //v
	sdlKeyCode[98] = 66; //b
	sdlKeyCode[110] = 78; //n
	sdlKeyCode[109] = 77; //m
	sdlKeyCode[44] = 188; //comma
	sdlKeyCode[46] = 190; //period
	sdlKeyCode[47] = 191; //slash
	sdlKeyCode[1073742053] = 16; //rightshift
	sdlKeyCode[1073742048] = 17; //leftctrl
	sdlKeyCode[1073742051] = 91; //leftwinkey
	sdlKeyCode[1073742050] = 18; //leftalt
	sdlKeyCode[32] = 32; //space
	sdlKeyCode[1073742054] = 18; //rightalt
	sdlKeyCode[1073741925] = 93; //menu
	sdlKeyCode[1073742052] = 17; //rightctrl
	sdlKeyCode[1073741906] = 38; //up
	sdlKeyCode[1073741905] = 40; //down
	sdlKeyCode[1073741904] = 37; //left
	sdlKeyCode[1073741903] = 39; //right

	//sdlCharCode translates (SDL_Event->key.keysym.sym) to charcode
	sdlCharCode[27] = 0; //esc
	sdlCharCode[1073741882] = 0; //f1
	sdlCharCode[1073741883] = 0; //f2
	sdlCharCode[1073741884] = 0; //f3
	sdlCharCode[1073741885] = 0; //f4
	sdlCharCode[1073741886] = 0; //f5
	sdlCharCode[1073741887] = 0; //f6
	sdlCharCode[1073741888] = 0; //f7
	sdlCharCode[1073741889] = 0; //f8
	sdlCharCode[1073741890] = 0; //f9
	sdlCharCode[1073741891] = 0; //f10
	sdlCharCode[1073741892] = 0; //f11
	sdlCharCode[1073741893] = 0; //f12
	sdlCharCode[1073741894] = 0; //printscreen
	sdlCharCode[1073741895] = 0; //scrolllock
	sdlCharCode[1073741896] = 0; //pause
	sdlCharCode[96] = 96; //tilde
	sdlCharCode[48] = 48; //0
	sdlCharCode[49] = 49; //1
	sdlCharCode[50] = 50; //2
	sdlCharCode[51] = 51; //3
	sdlCharCode[52] = 52; //4
	sdlCharCode[53] = 53; //5
	sdlCharCode[54] = 54; //6
	sdlCharCode[55] = 55; //7
	sdlCharCode[56] = 56; //8
	sdlCharCode[57] = 57; //9
	sdlCharCode[45] = 45; //minus
	sdlCharCode[61] = 61; //equal
	sdlCharCode[92] = 92; //backslash
	sdlCharCode[8] = 0; //backspace
	sdlCharCode[1073741901] = 0; //end
	sdlCharCode[1073741898] = 0; //home
	sdlCharCode[1073741907] = 0; //numlock
	sdlCharCode[1073741908] = 47; //kp_slash
	sdlCharCode[1073741909] = 42; //kp_multiply
	sdlCharCode[1073741910] = 45; //kp_minus
	sdlCharCode[9] = 0; //tab
	sdlCharCode[113] = 113; //q
	sdlCharCode[119] = 119; //w
	sdlCharCode[101] = 101; //e
	sdlCharCode[114] = 114; //r
	sdlCharCode[116] = 116; //t
	sdlCharCode[121] = 121; //y
	sdlCharCode[117] = 117; //u
	sdlCharCode[105] = 105; //i
	sdlCharCode[111] = 111; //o
	sdlCharCode[112] = 112; //p
	sdlCharCode[91] = 91; //leftbracket
	sdlCharCode[93] = 93; //rightbracket
	sdlCharCode[13] = 13; //enter
	sdlCharCode[127] = 0; //delete
	sdlCharCode[1073741897] = 0; //insert
	sdlCharCode[1073741899] = 0; //pageup
	sdlCharCode[1073741902] = 0; //pagedown
	sdlCharCode[1073741922] = 48; //kp_0
	sdlCharCode[1073741913] = 49; //kp_1
	sdlCharCode[1073741914] = 50; //kp_2
	sdlCharCode[1073741915] = 51; //kp_3
	sdlCharCode[1073741916] = 52; //kp_4
	sdlCharCode[1073741917] = 53; //kp_5
	sdlCharCode[1073741918] = 54; //kp_6
	sdlCharCode[1073741919] = 55; //kp_7
	sdlCharCode[1073741920] = 56; //kp_8
	sdlCharCode[1073741921] = 57; //kp_9
	sdlCharCode[1073741923] = 46; //kp_period
	sdlCharCode[1073741911] = 43; //kp_plus
	sdlCharCode[1073741912] = 13; //kp_enter
	sdlCharCode[1073741881] = 0; //capslock
	sdlCharCode[97] = 97; //a
	sdlCharCode[115] = 115; //s
	sdlCharCode[100] = 100; //d
	sdlCharCode[102] = 102; //f
	sdlCharCode[103] = 103; //g
	sdlCharCode[104] = 104; //h
	sdlCharCode[106] = 106; //j
	sdlCharCode[107] = 107; //k
	sdlCharCode[108] = 108; //l
	sdlCharCode[59] = 59; //semicolon
	sdlCharCode[39] = 39; //quote
	sdlCharCode[1073742049] = 0; //leftshift
	sdlCharCode[122] = 122; //z
	sdlCharCode[120] = 120; //x
	sdlCharCode[99] = 99; //c
	sdlCharCode[118] = 118; //v
	sdlCharCode[98] = 98; //b
	sdlCharCode[110] = 110; //n
	sdlCharCode[109] = 109; //m
	sdlCharCode[44] = 44; //comma
	sdlCharCode[46] = 46; //period
	sdlCharCode[47] = 47; //slash
	sdlCharCode[1073742053] = 0; //rightshift
	sdlCharCode[1073742048] = 0; //leftctrl
	sdlCharCode[1073742051] = 0; //leftwinkey
	sdlCharCode[1073742050] = 0; //leftalt
	sdlCharCode[32] = 32; //space
	sdlCharCode[1073742054] = 0; //rightalt
	sdlCharCode[1073741925] = 0; //menu
	sdlCharCode[1073742052] = 0; //rightctrl
	sdlCharCode[1073741906] = 0; //up
	sdlCharCode[1073741905] = 0; //down
	sdlCharCode[1073741904] = 0; //left
	sdlCharCode[1073741903] = 0; //right

	//sdlShiftCharCode translates (SDL_Event->key.keysym.sym) to shifted charcode
	sdlShiftCharCode[27] = 0; //esc
	sdlShiftCharCode[1073741882] = 0; //f1
	sdlShiftCharCode[1073741883] = 0; //f2
	sdlShiftCharCode[1073741884] = 0; //f3
	sdlShiftCharCode[1073741885] = 0; //f4
	sdlShiftCharCode[1073741886] = 0; //f5
	sdlShiftCharCode[1073741887] = 0; //f6
	sdlShiftCharCode[1073741888] = 0; //f7
	sdlShiftCharCode[1073741889] = 0; //f8
	sdlShiftCharCode[1073741890] = 0; //f9
	sdlShiftCharCode[1073741891] = 0; //f10
	sdlShiftCharCode[1073741892] = 0; //f11
	sdlShiftCharCode[1073741893] = 0; //f12
	sdlShiftCharCode[1073741894] = 0; //printscreen
	sdlShiftCharCode[1073741895] = 0; //scrolllock
	sdlShiftCharCode[1073741896] = 0; //pause
	sdlShiftCharCode[96] = 126; //tilde
	sdlShiftCharCode[48] = 41; //0
	sdlShiftCharCode[49] = 33; //1
	sdlShiftCharCode[50] = 64; //2
	sdlShiftCharCode[51] = 35; //3
	sdlShiftCharCode[52] = 36; //4
	sdlShiftCharCode[53] = 37; //5
	sdlShiftCharCode[54] = 94; //6
	sdlShiftCharCode[55] = 38; //7
	sdlShiftCharCode[56] = 42; //8
	sdlShiftCharCode[57] = 40; //9
	sdlShiftCharCode[45] = 95; //minus
	sdlShiftCharCode[61] = 43; //equal
	sdlShiftCharCode[92] = 124; //backslash
	sdlShiftCharCode[8] = 0; //backspace
	sdlShiftCharCode[1073741901] = 0; //end
	sdlShiftCharCode[1073741898] = 0; //home
	sdlShiftCharCode[1073741907] = 0; //numlock
	sdlShiftCharCode[1073741908] = 47; //kp_slash
	sdlShiftCharCode[1073741909] = 42; //kp_multiply
	sdlShiftCharCode[1073741910] = 45; //kp_minus
	sdlShiftCharCode[9] = 0; //tab
	sdlShiftCharCode[113] = 81; //q
	sdlShiftCharCode[119] = 87; //w
	sdlShiftCharCode[101] = 69; //e
	sdlShiftCharCode[114] = 82; //r
	sdlShiftCharCode[116] = 84; //t
	sdlShiftCharCode[121] = 89; //y
	sdlShiftCharCode[117] = 85; //u
	sdlShiftCharCode[105] = 73; //i
	sdlShiftCharCode[111] = 79; //o
	sdlShiftCharCode[112] = 80; //p
	sdlShiftCharCode[91] = 123; //leftbracket
	sdlShiftCharCode[93] = 125; //rightbracket
	sdlShiftCharCode[13] = 13; //enter
	sdlShiftCharCode[127] = 0; //delete
	sdlShiftCharCode[1073741897] = 0; //insert
	sdlShiftCharCode[1073741899] = 0; //pageup
	sdlShiftCharCode[1073741902] = 0; //pagedown
	sdlShiftCharCode[1073741922] = 0; //kp_0
	sdlShiftCharCode[1073741913] = 0; //kp_1
	sdlShiftCharCode[1073741914] = 0; //kp_2
	sdlShiftCharCode[1073741915] = 0; //kp_3
	sdlShiftCharCode[1073741916] = 0; //kp_4
	sdlShiftCharCode[1073741917] = 0; //kp_5
	sdlShiftCharCode[1073741918] = 0; //kp_6
	sdlShiftCharCode[1073741919] = 0; //kp_7
	sdlShiftCharCode[1073741920] = 0; //kp_8
	sdlShiftCharCode[1073741921] = 0; //kp_9
	sdlShiftCharCode[1073741923] = 0; //kp_period
	sdlShiftCharCode[1073741911] = 43; //kp_plus
	sdlShiftCharCode[1073741912] = 13; //kp_enter
	sdlShiftCharCode[1073741881] = 0; //capslock
	sdlShiftCharCode[97] = 65; //a
	sdlShiftCharCode[115] = 83; //s
	sdlShiftCharCode[100] = 68; //d
	sdlShiftCharCode[102] = 70; //f
	sdlShiftCharCode[103] = 71; //g
	sdlShiftCharCode[104] = 72; //h
	sdlShiftCharCode[106] = 74; //j
	sdlShiftCharCode[107] = 75; //k
	sdlShiftCharCode[108] = 76; //l
	sdlShiftCharCode[59] = 58; //semicolon
	sdlShiftCharCode[39] = 34; //quote
	sdlShiftCharCode[1073742049] = 0; //leftshift
	sdlShiftCharCode[122] = 90; //z
	sdlShiftCharCode[120] = 88; //x
	sdlShiftCharCode[99] = 67; //c
	sdlShiftCharCode[118] = 86; //v
	sdlShiftCharCode[98] = 66; //b
	sdlShiftCharCode[110] = 78; //n
	sdlShiftCharCode[109] = 77; //m
	sdlShiftCharCode[44] = 60; //comma
	sdlShiftCharCode[46] = 62; //period
	sdlShiftCharCode[47] = 63; //slash
	sdlShiftCharCode[1073742053] = 0; //rightshift
	sdlShiftCharCode[1073742048] = 0; //leftctrl
	sdlShiftCharCode[1073742051] = 0; //leftwinkey
	sdlShiftCharCode[1073742050] = 0; //leftalt
	sdlShiftCharCode[32] = 32; //space
	sdlShiftCharCode[1073742054] = 0; //rightalt
	sdlShiftCharCode[1073741925] = 0; //menu
	sdlShiftCharCode[1073742052] = 0; //rightctrl
	sdlShiftCharCode[1073741906] = 0; //up
	sdlShiftCharCode[1073741905] = 0; //down
	sdlShiftCharCode[1073741904] = 0; //left
	sdlShiftCharCode[1073741903] = 0; //right

	//sdlMacCode translates (SDL_Event->key.keysym.sym) to native mac keycode
	sdlMacCode[27] = 53; //esc
	sdlMacCode[1073741882] = 122; //f1
	sdlMacCode[1073741883] = 120; //f2
	sdlMacCode[1073741884] = 99; //f3
	sdlMacCode[1073741885] = 118; //f4
	sdlMacCode[1073741886] = 96; //f5
	sdlMacCode[1073741887] = 97; //f6
	sdlMacCode[1073741888] = 98; //f7
	sdlMacCode[1073741889] = 100; //f8
	sdlMacCode[1073741890] = 101; //f9
	sdlMacCode[1073741891] = 109; //f10
	sdlMacCode[1073741892] = 103; //f11
	sdlMacCode[1073741893] = 111; //f12
	sdlMacCode[1073741894] = 0; //printscreen
	sdlMacCode[1073741895] = 0; //scrolllock    TODO:
	sdlMacCode[1073741896] = 0; //pause         TODO:
	sdlMacCode[96] = 50; //tilde
	sdlMacCode[48] = 29; //0
	sdlMacCode[49] = 18; //1
	sdlMacCode[50] = 19; //2
	sdlMacCode[51] = 20; //3
	sdlMacCode[52] = 21; //4
	sdlMacCode[53] = 23; //5
	sdlMacCode[54] = 22; //6
	sdlMacCode[55] = 26; //7
	sdlMacCode[56] = 28; //8
	sdlMacCode[57] = 25; //9
	sdlMacCode[45] = 27; //minus
	sdlMacCode[61] = 24; //equal
	sdlMacCode[92] = 42; //backslash
	sdlMacCode[8] = 51; //backspace
	sdlMacCode[1073741901] = 119; //end
	sdlMacCode[1073741898] = 115; //home
	sdlMacCode[1073741907] = 0; //numlock      TODO:
	sdlMacCode[1073741908] = 75; //kp_slash
	sdlMacCode[1073741909] = 67; //kp_multiply
	sdlMacCode[1073741910] = 78; //kp_minus
	sdlMacCode[9] = 48; //tab
	sdlMacCode[113] = 12; //q
	sdlMacCode[119] = 13; //w
	sdlMacCode[101] = 14; //e
	sdlMacCode[114] = 15; //r
	sdlMacCode[116] = 17; //t
	sdlMacCode[121] = 16; //y
	sdlMacCode[117] = 32; //u
	sdlMacCode[105] = 34; //i
	sdlMacCode[111] = 31; //o
	sdlMacCode[112] = 35; //p
	sdlMacCode[91] = 33; //leftbracket
	sdlMacCode[93] = 30; //rightbracket
	sdlMacCode[13] = 36; //enter
	sdlMacCode[127] = 117; //delete
	sdlMacCode[1073741897] = 0; //insert    TODO:
	sdlMacCode[1073741899] = 116; //pageup
	sdlMacCode[1073741902] = 121; //pagedown
	sdlMacCode[1073741922] = 82; //kp_0
	sdlMacCode[1073741913] = 83; //kp_1
	sdlMacCode[1073741914] = 84; //kp_2
	sdlMacCode[1073741915] = 85; //kp_3
	sdlMacCode[1073741916] = 86; //kp_4
	sdlMacCode[1073741917] = 87; //kp_5
	sdlMacCode[1073741918] = 88; //kp_6
	sdlMacCode[1073741919] = 89; //kp_7
	sdlMacCode[1073741920] = 91; //kp_8
	sdlMacCode[1073741921] = 92; //kp_9
	sdlMacCode[1073741923] = 65; //kp_period
	sdlMacCode[1073741911] = 69; //kp_plus
	sdlMacCode[1073741912] = 76; //kp_enter
	sdlMacCode[1073741881] = 57; //capslock
	sdlMacCode[97] = 0; //a
	sdlMacCode[115] = 1; //s
	sdlMacCode[100] = 2; //d
	sdlMacCode[102] = 3; //f
	sdlMacCode[103] = 5; //g
	sdlMacCode[104] = 4; //h
	sdlMacCode[106] = 38; //j
	sdlMacCode[107] = 40; //k
	sdlMacCode[108] = 37; //l
	sdlMacCode[59] = 41; //semicolon
	sdlMacCode[39] = 39; //quote
	sdlMacCode[1073742049] = 56; //leftshift
	sdlMacCode[122] = 6; //z
	sdlMacCode[120] = 7; //x
	sdlMacCode[99] = 8; //c
	sdlMacCode[118] = 9; //v
	sdlMacCode[98] = 11; //b
	sdlMacCode[110] = 45; //n
	sdlMacCode[109] = 46; //m
	sdlMacCode[44] = 43; //comma
	sdlMacCode[46] = 47; //period
	sdlMacCode[47] = 44; //slash
	sdlMacCode[1073742053] = 60; //rightshift
	sdlMacCode[1073742048] = 59; //leftctrl
	sdlMacCode[1073742051] = 0; //leftwinkey   TODO: Command=55 Option=58
	sdlMacCode[1073742050] = 0; //leftalt      TODO: Command=55 Option=58
	sdlMacCode[32] = 49; //space
	sdlMacCode[1073742054] = 0; //rightalt      TODO: RightOption=61
	sdlMacCode[1073741925] = 0; //menu         TODO:
	sdlMacCode[1073742052] = 62; //rightctrl
	sdlMacCode[1073741906] = 126; //up
	sdlMacCode[1073741905] = 125; //down
	sdlMacCode[1073741904] = 123; //left
	sdlMacCode[1073741903] = 124; //right
}
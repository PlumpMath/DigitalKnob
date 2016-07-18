///////////////////////////
function DKMenuRight_Init()
{
	DKCreate("DKDev/DKMenuRight.html");
	DKAddEvent("GLOBAL", "keydown", DKMenuRight_OnEvent);
	DKAddEvent("DevModeButton", "click", DKMenuRight_OnEvent);
	DKAddEvent("AppImage", "click", DKMenuRight_OnEvent);
	DKAddEvent("Image19", "click", DKMenuRight_OnEvent);
	DKAddEvent("Image20", "click", DKMenuRight_OnEvent);
	DKAddEvent("WebImage", "click", DKMenuRight_OnEvent);
	DKAddEvent("TriggerImage", "click", DKMenuRight_OnEvent);
	DKAddEvent("HookImage", "click", DKMenuRight_OnEvent);
	DKAddEvent("DKMenuRight.html", "SetPanel", DKMenuRight_OnEvent);
	
	DKCreate("DKDev/DKMenuRightApp.js");
	DKCreate("DKDev/DKMenuRightEdit.js");
}

//////////////////////////
function DKMenuRight_End()
{
	DKRemoveEvent("GLOBAL", "keydown", DKMenuRight_OnEvent);
	DKClose("DKMenuRight.html");
}

///////////////////////////////////
function DKMenuRight_OnEvent(event)
{
	if(DK_Id(event, "AppImage")){
		DKMenuRight_SetPanel("App");
	}
	if(DK_Id(event, "Image19")){
		DKCreate("DKDev/DKMenuRightEdit.js");
		DKMenuRight_SetPanel("Edit");
	}
	if(DK_Id(event, "Image20")){
		DKCreate("DKDev/DKMenuRightTree.js");
		DKMenuRight_SetPanel("Tree");
	}
	if(DK_Id(event, "WebImage")){
		DKCreate("DKDev/DKMenuRightWeb.js");
		DKMenuRight_SetPanel("Web");
	}
	if(DK_Id(event, "TriggerImage")){
		DKCreate("DKTriggers/DKAutomate.js");
		DKFrame_Widget("DKAutomate.html");
	}
	if(DK_Id(event, "HookImage")){
		DKCreate("DKHook/DKHook.js");
		DKFrame_Widget("DKHook.html");
	}
	if(DK_Id(event, "DevModeButton")){
		DKMenuRight_ToggleEditMode();
	}
	if(DK_Type(event, "SetPanel")){
		DKLog("DKMenuRight_OnEvent("+event+") \n", DKDEBUG);
		DKMenuRight_SetPanel(DKWidget_GetValue(event));
	}
}

/////////////////////////////////////
function DKMenuRight_SetPanel(string)
{
	DKWidget_Hide("DKMenuRightApp.html");
	DKWidget_Hide("DKMenuRightEdit.html");
	DKWidget_Hide("DKMenuRightTree.html");
	DKWidget_Hide("DKMenuRightWeb.html");
	DKWidget_SetAttribute("AppImage", "src", "DKDev/app.png");
	DKWidget_SetAttribute("Image19", "src", "DKDev/pencil.png");
	DKWidget_SetAttribute("Image20", "src", "DKDev/tree.png");
	DKWidget_SetAttribute("WebImage", "src", "DKDev/web.png");

	if(string == "App"){
		DKWidget_SetAttribute("AppImage", "src", "DKDev/app-select.png");
		DKWidget_Show("DKMenuRightApp.html");
	}
	if(string == "Edit"){
		DKWidget_SetAttribute("Image19", "src", "DKDev/pencil-select.png");
		DKWidget_Show("DKMenuRightEdit.html");
	}
	if(string == "Tree"){
		DKWidget_SetAttribute("Image20", "src", "DKDev/tree-select.png");
		DKWidget_Show("DKMenuRightTree.html");
	}
	if(string == "Web"){
		DKWidget_SetAttribute("WebImage", "src", "DKDev/web-select.png");
		DKWidget_Show("DKMenuRightWeb.html");
	}
}

/////////////////////////////////////
function DKMenuRight_ToggleEditMode()
{
	DKLog("DKMenuRight_ToggleEditMode() \n", DKDEBUG);
	var img = DKWidget_GetAttribute("DevModeButton", "src");
	if(img.indexOf("greenbutton.png") != -1){
		DKMenuRight_EditModeOn();
		return;
	}
	if(img.indexOf("redbutton.png") != -1){
		DKMenuRight_EditModeOff();
	}
}

/////////////////////////////////
function DKMenuRight_EditModeOn()
{
	DKLog("DKMenuRight_EditModeOn() \n", DKDEBUG);
	DKWidget_SetAttribute("DevModeButton", "src", "DKDev/redbutton.png");
	DKCreate("DKDev/DKDev.js");
	DKDev_On();
	DKSendEvent("GLOBAL", "ToggleTriggers", "OFF");
}

//////////////////////////////////
function DKMenuRight_EditModeOff()
{
	DKLog("DKMenuRight_EditModeOff() \n", DKDEBUG);
	DKWidget_SetAttribute("DevModeButton", "src", "DKDev/greenbutton.png");
	DKCreate("DKDev/DKDev.js");
	DKDev_Off();
	DKSendEvent("GLOBAL", "ToggleTriggers", "ON");
}

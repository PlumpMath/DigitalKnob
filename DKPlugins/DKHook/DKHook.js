//////////////////////
function DKHook_Init()
{
	DKCreate("DKHook");
	DKCreate("DKHook/DKHook.html");
	DKAddEvent("getvalue", "click", DKHook_OnEvent);
	DKAddEvent("setvalue", "click", DKHook_OnEvent);
	DKAddEvent("doclick", "click", DKHook_OnEvent);
	DKAddEvent("Prev", "click", DKHook_OnEvent);
	DKAddEvent("Next", "click", DKHook_OnEvent);
	DKAddEvent("Highlight", "click", DKHook_OnEvent);
	DKHook_UpdateWindowList();
}

/////////////////////
function DKHook_End()
{
	DKRemoveEvents(DKHook_OnEvent);
	DKClose("DKHook/DKHook.html");
}

//////////////////////////////
function DKHook_OnEvent(event)
{
	DKLog("DKHook_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
	
	if(DK_Id(event, "getvalue")){
		var value = DKHook_GetValue();
		DKWidget_SetValue("GetValueBox", value);
    }
	if(DK_Id(event, "setvalue")){
		var value = DKWidget_GetValue("SetValueBox");
		DKHook_SetValue(value);
    }
	if(DK_Id(event, "doclick")){
		DKHook_Click();
    }
	if(DK_Id(event, "Prev")){
		DKHook_PrevHandle();
	}
	if(DK_Id(event, "Next")){
		DKHook_NextHandle();
	}
	if(DK_Id(event, "Highlight")){
		DKHook_ToggleHighlight();
	}
	if(DK_IdLike(event, "wintitle")){
		//DKLog("DKHook_OnEvent("+event+")\n");
		//DKLog("DKHook_OnEvent(): id = "+DK_GetId(event)+"\n");
		//DKLog("DKHook_OnEvent(): value = "+DKWidget_GetValue(DK_GetId(event))+"\n");
		DKHook_SetWindowHandle(DKWidget_GetValue(DK_GetId(event)));
	}

	DKWidget_SetValue("currentHandle", DKHook_CurrentHandle());
}

//////////////////////////////////
function DKHook_UpdateWindowList()
{
	var str = DKHook_GetWindows();
	if(!str){ return; }
	DKLog("WINDOW LIST: "+str+"\n");
	var arry = str.split(",");
	
	//RemoveDuplicates(arry);
	for(var i=0; i<arry.length; i++){
		var element = DKWidget_CreateElement("windows", "option", "wintitle");
		DKWidget_SetAttribute(element, "value", arry[i]);
		DKWidget_SetProperty(element, "white-space", "nowrap");
		DKAddEvent(element, "click", DKHook_OnEvent);
		DKWidget_SetInnerHtml(element,arry[i]);
	}
}
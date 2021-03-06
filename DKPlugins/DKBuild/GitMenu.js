///////////////////////
function GitMenu_Init()
{
	DKCreate("DKBuild/GitMenu.html", function(){});
	DKWidget_SetProperty("GitMenu.html","top",DKWindow_GetMouseY()+"px");
	DKWidget_SetProperty("GitMenu.html","left",DKWindow_GetMouseX()+"px");
	DKAddEvent("GLOBAL", "mousedown", GitMenu_OnEvent);
	DKAddEvent("Git Update", "click", GitMenu_OnEvent);
	DKAddEvent("Git Commit", "click", GitMenu_OnEvent);
}

//////////////////////
function GitMenu_End()
{
	DKLog("GitMenu_End()");
	
	DKRemoveEvent("GLOBAL", "mousedown", GitMenu_OnEvent);
	DKRemoveEvents(GitMenu_OnEvent);
	DKClose("DKBuild/GitMenu.html");
}

///////////////////////////////
function GitMenu_OnEvent(event)
{
	DKLog("GitMenu_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
	
	if(DK_Id(event,"Git Update")){
		DKThread_DKQueue("GitUpdate","DKBuild_GitUpdate();");
	}
	if(DK_Id(event,"Git Commit")){
		DKThread_DKQueue("GitCommit","DKBuild_GitCommit();");
	}
	
	if(DK_Id(event, "GLOBAL")){
		if(DKWidget_IsChildOf(DKWidget_GetHoverElement(), "GitMenu.html")){
			return;
		}
	}
	DKClose("DKBuild/GitMenu.js");
}
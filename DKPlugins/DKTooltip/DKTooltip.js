/////////////////////////
function DKTooltip_Init()
{
	DKCreate("DKTooltip/DKTooltip.html");
	DKAddEvent("DKTooltip.html", "show_tooltip", DKTooltip_OnEvent);
	//DKAddEvent("DKTooltip.html", "mouseover", DKTooltip_OnEvent);
	DKAddEvent("DKTooltip.html", "mouseout", DKTooltip_OnEvent);
}

////////////////////////
function DKTooltip_End()
{
	DKRemoveEvent("DKTooltip.html", "show_tooltip", DKTooltip_OnEvent);
	//DKRemoveEvent("DKTooltip.html", "mouseover", DKTooltip_OnEvent);
	DKRemoveEvent("DKTooltip.html", "mouseout", DKTooltip_OnEvent);
	DKClose("DKTooltip.html");
}

/////////////////////////////////
function DKTooltip_OnEvent(event)
{
	DKLog("DKTooltip_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
	
	if(DK_Type(event, "mouseout")){
		DKClose("DKTooltip/DKTooltip.js");
	}
	if(DK_Type(event, "show_tooltip")){
		//Get mouse x, y
		DKWidget_SetProperty("DKTooltip.html", "top", mouseX + "px");
		DKWidget_SetProperty("DKTooltip.html", "left", mouseY + "px");
		DKWidget_SetInnerHtml("DKTooltipText", DK_GetValue(event));
	}
}

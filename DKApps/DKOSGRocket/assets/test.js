DKCreate("DKWidgetJS");
DKCreate("DKWidget,test.html");

////////////////////
function test_Init()
{
	DKAddEvent("GLOBAL", "keydown", test_OnEvent);
}

////////////////////////////
function test_OnEvent(event)
{
	if(DK_Type(event,"keydown")){
		DKLog("keydown "+DKWidget_GetValue(event)+" \n");
		if(DKWidget_GetValue(event) == "4"){ //ANDROID_BACK
			DK_Exit();
		}
	}
}
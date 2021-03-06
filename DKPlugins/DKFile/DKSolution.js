//////////////////////////
function DKSolution_Init()
{	
	DKCreate("DKFile/DKSolution.html");
	DKAddEvent("DKSolutionUp", "click", DKSolution_OnEvent);
	DKAddEvent("DKSolutionMenu", "click", DKSolution_OnEvent);
	DKAddEvent("DKSolutionMenu", "contextmenu", DKSolution_OnEvent);
	
	if(typeof DKPATH !== 'undefined'){
		DKSolution_UpdatePath(DKPATH); //DKPATH from DKFile.js
	}
}

/////////////////////////
function DKSolution_End()
{
	DKRemoveEvents(DKSolution_OnEvent);
	DKClose("DKFile/DKSolution.html");
}

//////////////////////////////////
function DKSolution_OnEvent(event)
{	
	//DKLog("DKSolution_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
	
	DKSolution_Select(DK_GetId(event));

	if(DK_Type(event, "click")){
		DK_StopPropagation(event);
	}
	
	if(DK_Type(event, "contextmenu")){
		//DKLog("DKSolution_OnEvent() contextmenu\n");	
		DK_StopPropagation(event);
		DKCreate("DKFile/DKSolutionMenu.js", function(){
			var file = DKWidget_GetValue(DK_GetId(event));
			if(!file){
				file = DKWidget_GetValue("DKSolutionPath")+"/";
			}
			DKSolutionMenu_SetId(DK_GetId(event));
			DKSolutionMenu_SetFile(file);
		});
		return;
	}
		
	if(DK_Id(event, "DKSolutionUp")){
		var up = DKWidget_GetValue("DKSolutionPath")+"/..";
		//DKLog(up+"\n");
		DKSolution_OpenFolder(up);
	}
	
	if(DK_Type(event, "dblclick")){
		//DKLog(DK_GetId(event)+"\n");
		//DKLog(DKWidget_GetValue(DK_GetId(event))+"\n");
		if(DK_IdLike(event, "DKSolutionFolder")){
			//DKLog("DKSolutionFolder");
			DKSolution_OpenFolder(DKWidget_GetValue(DK_GetId(event)));
			return;
		}
	
		DKSolution_OpenFile(DKWidget_GetValue(DK_GetId(event)));
		//DK_ClearSelection();
		return;
	}
}

//////////////////////////////
function DKSolution_Select(id)
{
	var elements = DKWidget_GetElements("DKSolutionMenu");
	var arry = elements.split(",");
	for(var i=0; i<arry.length-1; i++){
		if(!arry[i]){
			DKLog("DKSolution_Select(id): arry["+i+"] invalid\n", DKERROR);
		}
		DKWidget_SetProperty(arry[i], "background-color", "rgb(255,255,255)");
		DKWidget_SetProperty(arry[i], "color", "rgb(0,0,0)");
	}
	if(id.indexOf("DKSolutionFolder") > -1 || id.indexOf("DKSolutionFile") > -1){
		DKWidget_SetProperty(id, "background-color", "rgb(123,157,212)");
		DKWidget_SetProperty(id, "color", "rgb(255,255,255)");
	}
}

////////////////////////////////////
function DKSolution_OpenFolder(path)
{
	//DKLog("DKSolution_OpenFolder("+path+") \n");
	if(DKSolution_UpdatePath(path)){
		return true;
	}
	return false;
}

//////////////////////////////////
function DKSolution_OpenFile(path)
{
	//DKLog("DKSolution_OpenFile("+path+") \n");
	var aPath = path;
	if(DK_GetOS() != "Android"){
		aPath = DKFile_GetAbsolutePath(path);
	}
	//DKLog("aPath:"+aPath+"\n");
	if(!DK_Run(aPath)){ return false; }
	return true;
}

//////////////////////////////////
function DKSolution_OpenHere(path)
{
	//DKLog("DKSolution_OpenHere("+path+") \n");
	var aPath = path;
	if(DK_GetOS() != "Android"){
		aPath = DKFile_GetAbsolutePath(path);
	}
	//DKLog("aPath:"+aPath+"\n");
	if(DKFile_IsDirectory(aPath)){ //Folder
		if(!DKSolution_UpdatePath(aPath)){ return false; }
		return true;
	}
	else{ //File
		DKCreate("DKFileAssociation/DKFileAssociation.js", function(){
			if(!DKFileAssociation_Open(aPath)){ return false; }
			return true;
		});
	}
	
	return false; //error
}

////////////////////////////////////
function DKSolution_UpdatePath(path)
{
	//if(!path){ return false; }
	//DKLog("DKSolution_UpdatePath("+path+") \n");
	var aPath = path;
	if(DK_GetOS() != "Android"){
		aPath = DKFile_GetAbsolutePath(path);
	}
	//DKLog("aPath:"+aPath+"\n");
	//var rPath = DKFile_GetRelativePath(aPath, path);
	//DKLog("rPath:"+rPath+"\n");
	
	var temp = DKFile_DirectoryContents(aPath);
	if(!temp){ return false; }
	var files = temp.split(",");

	DKWidget_SetInnerHtml("DKSolutionMenu", ""); //Clear it

	for(var d=0; d<files.length; d++){
		if(DKFile_IsDirectory(aPath+"/"+files[d])){ //Folders
			var element2 = DKWidget_CreateElement("DKSolutionMenu", "option", "DKSolutionFolder");
			var value = aPath+"/"+files[d];
			DKWidget_SetAttribute(element2,"value", value);
			DKWidget_SetProperty(element2, "white-space", "nowrap");
			DKAddEvent(element2, "click", DKSolution_OnEvent);
			DKWidget_SetProperty(element2, "padding-left", "17px");
			DKWidget_SetInnerHtml(element2,files[d]);
			DKWidget_SetProperty(element2, "background-image", "url(\"DKFile/folder.png\")");
			DKWidget_SetProperty(element2, "background-repeat", "no-repeat");
			DKAddEvent(element2, "click", DKSolution_OnEvent);
			DKAddEvent(element2, "dblclick", DKSolution_OnEvent);
			DKAddEvent(element2, "contextmenu", DKSolution_OnEvent);
		}
	}

	for(var f=0; f<files.length; f++){
		if(!DKFile_IsDirectory(aPath+"/"+files[f])){ //Files
			var element3 = DKWidget_CreateElement("DKSolutionMenu", "option", "DKSolutionFile");
			var value = aPath+"/"+files[f];
			DKWidget_SetAttribute(element3, "value", value);
			DKWidget_SetProperty(element3, "white-space", "nowrap");
			DKWidget_SetProperty(element3, "padding-left", "17px");
			DKWidget_SetProperty(element3, "background-repeat", "no-repeat");
			DKWidget_SetInnerHtml(element3,files[f]);
			DKAddEvent(element3, "click", DKSolution_OnEvent);
			DKAddEvent(element3, "dblclick", DKSolution_OnEvent);
			DKAddEvent(element3, "contextmenu", DKSolution_OnEvent);

			var extension = DKFile_GetExtention(files[f]);
			if((extension == "png") || (extension == "jpeg") || (extension == "jpg") || 
				(extension == "bmp") || (extension == "tiff") || (extension == "tif") || 
				(extension == "gif") || (extension == "tga") || (extension == "ico")
				){
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/picture.png\")");
			}
			else if((extension == "osg") || (extension == "osgb") || (extension == "osgt") ||
				(extension == "3dm") || (extension == "3ds") || (extension == "ac") ||
				(extension == "ascii") || (extension == "blend")  || (extension == "bvh") ||
				(extension == "c4d") || (extension == "dae") || (extension == "dds") ||
				(extension == "dgn") || (extension == "dwg") || (extension == "dxf") ||
				(extension == "fbx") || (extension == "lwo") || (extension == "lws") ||
				(extension == "ma") || (extension == "max") || (extension == "mb") ||
				(extension == "mesh") || (extension == "mtl") || (extension == "obj") ||
				(extension == "pov") || (extension == "skp") || (extension == "stl") ||
				(extension == "ztl")
			){
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/cube.png\")");
			}
			else if((extension == "js")){
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/js.png\")");
			}
			else if((extension == "sln")){
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/sln.png\")");
			}
			else if((extension == "html") || (extension == "htm")){
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/html.png\")");
			}
			else{
				DKWidget_SetProperty(element3, "background-image", "url(\"DKFile/file.png\")");
			}
		}
	}
	
	DKWidget_SetValue("DKSolutionPath", aPath);
	return true;
}


#include "stdafx.h"
#ifndef WIN32
#include "DKUnix.h"
#include <unistd.h>

////////////////////
int DKUnix::GetKey()
{
	DKLog("Press any key to continue...\n", DKINFO);
	return getchar();
}

////////////////////////////////////
bool DKUnix::Sleep(int milliseconds)
{
	usleep(milliseconds);
	return true;
}

////////////////////////////////////////////
bool DKUnix::GetUsername(DKString& username)
{
#ifdef LINUX
	char szUserName[64] = {0};
	int nGet = getlogin_r(szUserName, sizeof(szUserName)-1);
	if(0 != nGet){
		DKLog("DKUnix::GetUsername() failed. \n", DKERROR);
		return false;
    }
	DKLog("USERNAME: "+DKString(szUserName)+"\n", DKINFO);
	char* szHome = getlogin();
	DKLog("LOGIN: "+DKString(szHome)+"\n", DKINFO);

	username = szUserName;
	return true;
#endif
	return false;
}


#endif //!WIN32
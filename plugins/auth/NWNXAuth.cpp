/***************************************************************************
    NWNXAuth.cpp - Implementation of the CNWNXAuth class.
    (c) 2016 virusman (virusman@virusman.ru)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#include <string.h>
#include <string>
#include <stdlib.h>

#include "NWNXAuth.h"
#include "AuthHooks.h"
#include "NWNXApi.h"

extern PLUGINLINK *pluginLink;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNWNXAuth::CNWNXAuth()
{
    confKey = "AUTH";
    bHooked = 0;
}

CNWNXAuth::~CNWNXAuth()
{
}


bool CNWNXAuth::OnCreate(gline *config, const char *LogDir)
{
    char log[128];
    sprintf(log, "%s/nwnx_auth.txt", LogDir);

    // call the base class function
    if (!CNWNXBase::OnCreate(config, log))
        return false;
    Log(0, "NWNX Auth 1.0.0\n");
    Log(0, "(c) by virusman, 2016\n");
    Log(0, "visit us at http://www.nwnx.org\n\n");

    if (HookFunctions()) {
        bHooked = 1;
        Log(0, "* Module loaded successfully.\n");
    } else {
        bHooked = 0;
        Log(0, "* Module loaded successfully.\n");
        Log(0, "* Signature recognition failed. Some functions will be disabled.\n");
        //return false;
    }

    return true;
}

bool CNWNXAuth::HandlePlayerConnectRequest(const char *account, const char *cdkey)
{
    this->accountName = (char *) malloc(strlen(account) + 1);
    strcpy(this->accountName, account);

    this->cdkey = (char *) malloc(strlen(cdkey));
    strcpy(this->cdkey, cdkey);

    CExoString scriptName("on_auth");
    g_pVirtualMachine->RunScript(&scriptName, 0, 1);
    return g_pVirtualMachine->m_pReturnValue != 0;
}

char *CNWNXAuth::OnRequest(char *gameObject, char *Request, char *Parameters)
{
    Log(2, "Request: \"%s\"\n", Request);
    Log(3, "Params:  \"%s\"\n", Parameters);

    if (strncmp(Request, "GET_ACCOUNT", 11) == 0) {
        if (accountName != NULL) {
            char *ret = (char *) malloc(strlen(accountName) + 1);
            strcpy(ret, accountName);
            return ret;
        }
    }
    else if (strncmp(Request, "GET_CDKEY", 9) == 0) {
        if (cdkey != NULL) {
            char *ret = (char *) malloc(strlen(cdkey) + 1);
            strcpy(ret, cdkey);
            return ret;
        }
    }

    return NULL;
}

unsigned long CNWNXAuth::OnRequestObject(char *gameObject, char *Request)
{
    return OBJECT_INVALID;
}


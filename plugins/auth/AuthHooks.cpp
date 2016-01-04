/***************************************************************************
    Auth plugin for NWNX  - hooks implementation
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

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdarg.h>

#include <limits.h>     /* for PAGESIZE */

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#include "NWNXApi.h"
#include "AuthHooks.h"
#include "NWNXAuth.h"

extern CNWNXAuth plugin;

int (*CNetLayerInternal__HandleBNCSMessage_mid)();

CExoString *sCDKey = 0;
CExoString *sAccount = 0;

void CNetLayerInternal__HandleBNCSMessage_mid_Hook()
{
    asm("leave");
    asm("pusha");
    asm("lea -0x38(%ebp), %eax");
    asm("mov %eax, sCDKey");
    asm("lea -0x28(%ebp), %eax");
    asm("mov %eax, sAccount");
    plugin.Log(3, "BNCS: %s (%s)\n", sAccount->CStr(), sCDKey->CStr());

    if(!plugin.HandlePlayerConnectRequest(sAccount->CStr(), sCDKey->CStr()))
        *sCDKey = "x";

    plugin.Log(3, "cdkey: %s\n", sCDKey->CStr());

    asm("popa");
    asm("push CNetLayerInternal__HandleBNCSMessage_mid");
    asm("ret");
}

int HookFunctions()
{
    NX_HOOK(CNetLayerInternal__HandleBNCSMessage_mid, 0x082A2558,
            CNetLayerInternal__HandleBNCSMessage_mid_Hook, 6);
}

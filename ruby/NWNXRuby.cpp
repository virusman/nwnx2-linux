/***************************************************************************
    NWNXRuby.cpp - Implementation of the CNWNXFunction class.
    Copyright (C) 2003 Ingmar Stieger (Papillon)
    email: papillon@blackdagger.com

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
#include <sys/stat.h>

#include "NWNXRuby.h"
#include "FunctionHooks.h"
#include "ruby_int.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNWNXRuby::CNWNXRuby()
{
	confKey = "RUBY";
	bHooked = 1;
}

CNWNXRuby::~CNWNXRuby()
{
}

bool CNWNXRuby::OnCreate(gline *config, const char *LogDir)
{
	char log[128];
	struct stat statbuf;
	int i = 0;
	sprintf (log, "%s/nwnx_ruby.txt", LogDir);

	// call the base class function
	if (!CNWNXBase::OnCreate(config,log))
		return false;

	ruby_init();
	RubyInt_InitNWScript();

	Log(0,"NWNX Ruby V.1.0.0\n");
	Log(0,"(c) by virusman, 2008\n");
	if (HookFunctions())
	{
		bHooked=1;
		Log(0,"* Module loaded successfully.\n");
	}
	else
	{
		bHooked=0;
		Log(0,"* Module loaded successfully.\n");
		Log(0,"* Signature recognition failed. Some functions will be disabled.\n");
		//return false;
	}

	return true;
}

void CNWNXRuby::ExecuteCommand(char *value)
{
	VM_ExecuteCommand(0xF2);
	VM_ExecuteCommand(0xD);
}
char* CNWNXRuby::OnRequest (char *gameObject, char* Request, char* Parameters)
{
	Log(2,"(S) Request: \"%s\"\n",Request);
	Log(3,"(S) Params:  \"%s\"\n",Parameters);

	if (strncmp(Request, "EXECUTE", 7) == 0) 	
	{
		ExecuteCommand(Parameters);
		return NULL;
	}

	return NULL;
}
/*
unsigned long CNWNXRuby::OnRequestObject (char *gameObject, char* Request)
{
	return OBJECT_INVALID;
}
*/

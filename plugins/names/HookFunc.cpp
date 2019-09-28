/***************************************************************************
    Names plugin for NWNX  - hooks implementation
    (c) 2006 virusman (virusman@virusman.ru)

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

#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdarg.h>

#include <limits.h>		/* for PAGESIZE */
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#include "NWNXApi.h"
#include "HookFunc.h"
#include "CCustomNames.h"
#include "NWNXNames.h"

extern CNWNXNames names;

const dword PLAYERID_ALL_GAMEMASTERS = -10;

int (*pGetObjByOID)(void *pObjectClass, dword ObjID, void **buf);
void *(*pRetObjByOID)(void *pServerExo, long ObjID);
void *(*pGetPlayer)(void *pServerExo4, dword ObjID);
void (*pRunScript)();
char *(*d_newstrcpy)(char *buf, char **source);
void *(*GetPlayerObject)(void *pPlayer);
void *(*pGetPlayerByOID)(void *pThis, dword ObjID);
void *(*pGetPlayerList)(void *pServerExo);
void *(*pGetServerMessage)(void *pServerExo);
void *(*GetClientObjectByPlayerId)(void *pServerExoApp, dword nClientID, unsigned char flag);
void (*CNWMessage__CreateWriteMessage)(CNWSMessage *pMessage, dword length, dword recipient, dword flag);
void (*CNWSMessage__WriteGameObjUpdate_UpdateObject)(CNWSMessage *pMessage, CNWSPlayer *pPlayer, CNWSCreature *pCreature, CLastUpdateObject *pLUO, dword flags, dword AppearanceFlags);
void (*CNWMessage__GetWriteMessage)(CNWSMessage *pMessage, char **ppData, dword *pLength);
void (*CNWSMessage__SendServerToPlayerMessage)(CNWSMessage *pMessage, dword nPlayerID, byte type, byte subtype, char *dataPtr, dword length);
bool (*CNWSMessage__SendServerToPlayerPlayerList_Add)(CNWSMessage *pMessage, dword nPlayerId, CNWSPlayer *pNewPlayer);
bool (*CNWSMessage__SendServerToPlayerPlayerList_All)(CNWSMessage *pMessage, CNWSPlayer *pPlayer);

dword *ppServer = 0;
void *pServer = 0;
void *pServerExo = 0;
void *pServerExo4 = 0;
void *pScriptThis = 0;
void *pObjectClass = 0;
void *pFactionClass = 0;
void *pClientClass = 0;
dword oPC = 0;
int TRACK_CALLS = 1;

char scriptRun = 0;

unsigned char d_jmp_code[] = "\x68\x60\x70\x80\x90"       /* push dword 0x90807060 */
                             "\xc3\x90\x90\x90\x90";//x00 /* ret , nop , nop       */

unsigned char d_ret_code_nm[0x20];

//################################################################

void *GetObjectByID(dword ObjID)
{
    if (!pServerExo) InitConstants();
    void *pObject;
    pGetObjByOID(pObjectClass, ObjID, &pObject);
    return pObject;
}

void *GetPlayer(dword ObjID)
{
    if (!pServerExo) InitConstants();
    return pGetPlayerByOID(pServerExo, ObjID);
}

void *GetPlayerByClientID(dword nClientID)
{
    if (!pServerExo) InitConstants();
    return GetClientObjectByPlayerId(pServerExo, nClientID, 0);
}

int GetIsPC(dword ObjID)
{
    if (!pServerExo) InitConstants();
    void *pPlayer = GetPlayer(ObjID);
    if (pPlayer) return 1;
    else return 0;
}

int GetIsDM(dword ObjID)
{
    if (!pServerExo) InitConstants();
    CNWSCreature *pCreature = (CNWSCreature *) GetObjectByID(ObjID);
    if (!pCreature || !pCreature->CreatureStats) return 0;
    if (pCreature->CreatureStats->IsDM) return 1;
    else return 0;
}

void *GetPlayerList()
{
    if (!pServerExo) InitConstants();
    return pGetPlayerList(pServerExo);
}

void SendNewName(dword nPlayerObjID, dword nObjID)
{
    if (!pServerExo) InitConstants();
    CNWSMessage *pServerMessage = (CNWSMessage *) pGetServerMessage(pServerExo);
    CNWSPlayer *pPlayer = (CNWSPlayer *) GetPlayer(nPlayerObjID);
    CNWSObject *pObject = (CNWSObject *) GetObjectByID(nObjID);
    CLastUpdateObject luo;
    char *pData;
    dword length;
    if (!pServerMessage || !pPlayer || !pObject || pObject->ObjectType != 5) return;
    //pSendNewName(pServerMessage, pPlayer, pObject);
    CNWMessage__CreateWriteMessage(pServerMessage, 0x400, pPlayer->m_nPlayerID, 1);
    CNWSMessage__WriteGameObjUpdate_UpdateObject(pServerMessage, pPlayer, (CNWSCreature *) pObject, &luo, 0, 0x400);
    CNWMessage__GetWriteMessage(pServerMessage, &pData, &length);
    if (length) {
        CNWSMessage__SendServerToPlayerMessage(pServerMessage, pPlayer->m_nPlayerID, 5, 1, pData, length);
    }
}

void SendPlayerList(dword nPlayerObjID)
{
    if (!pServerExo) InitConstants();
    CNWSMessage *pServerMessage = (CNWSMessage *) pGetServerMessage(pServerExo);
    CNWSPlayer *pPlayer = (CNWSPlayer *) GetPlayer(nPlayerObjID);
    if (!pServerMessage || !pPlayer) return;
    CNWSMessage__SendServerToPlayerPlayerList_All(pServerMessage, pPlayer);
}

char *GetServerFuncName(dword Addr)
{
    /*if(Addr==0x080751CC) return "ReadAreaNames";
    if(Addr==0x0806A0F3) return "BuildCreatureMessage::CreatureFirstName";
    if(Addr==0x0806A10A) return "BuildCreatureMessage::CreatureLastName";
    if(Addr>=0x080714E8 && Addr<=0x08073543) return "BrainFuck2";
    if(Addr>=0x08073544 && Addr<=0x08073A5F) return "pcInput_ExamineCreature";
    if(Addr>=0x08073A60 && Addr<=0x08073FC8) return "pcInput_ExamineItem";
    if(Addr>=0x08069DB8 && Addr<=0x0806A01C) return "chat_ReadObjName";
    if(Addr>=0x08069B54 && Addr<=0x08069DB8) return "chat_ReadObjName2";
    if(Addr>=0x08069366 && Addr<=0x08069381) return "chat_ReadObjName5Private";
    if(Addr>=0x08069102 && Addr<=0x0806911D) return "chat_ReadObjName6";
    if(Addr>=0x080CE8D4 && Addr<=0x080CFD0C) return "LoadArea";
    if(Addr==0x08061D28) return "ReadInventoryItems";
    if(Addr==0x081B5E67) return "GetModuleName";
    if(Addr==0x08076A09 || Addr==0x08076A1C) return "Some_area_data";
    if(Addr==0x08076F2F || Addr==0x08076F40) return "Some_area_data2";
    if(Addr==0x08066403 || Addr==0x0806641A) return "CharList_ReadObj";
    if(Addr==0x0807DC17 || Addr==0x0807DC26) return "LoginCharList";*/

    if (Addr == 0x08066687 || Addr == 0x0806669E) 						return "CNWSMessage::SendServerToPlayerCharList";
    if (Addr == 0x081B8CEF) 											return "CNWSModule::PackModuleIntoMessage";
    if (Addr == 0x080D0FC1) 											return "CNWSArea::PackAreaIntoMessage";
    if (Addr == 0x08077301 || Addr == 0x08077314) 						return "CNWSMessage::SendServerToPlayerPlayerList_Add";
    if (Addr == 0x08077827 || Addr == 0x08077838) 						return "CNWSMessage::SendServerToPlayerPlayerList_All";
    if (Addr == 0x0806A377 || Addr == 0x0806A38E) 						return "CNWSMessage::WriteGameObjUpdate_UpdateAppearance";
    if (Addr == 0x08073AC1 || Addr == 0x08073BBE || Addr == 0x08073BDF) 	return "CNWSMessage::SendServerToPlayerExamineGui_CreatureData";
    if (Addr == 0x0806C245 || Addr == 0x0806C563) 						return "CNWSMessage::CreateNewLastUpdateObject";
    if (Addr == 0x08072DD3) 											return "CNWSMessage::WriteGameObjUpdate_UpdateObject*";
    if (Addr == 0x08073401 || Addr == 0x08073418 ||
            Addr == 0x080732AF || Addr == 0x080732C9) 						return "CNWSMessage::WriteGameObjUpdate_UpdateObject";
    if (Addr == 0x08061FAC) 											return "CNWSMessage::AddActiveItemPropertiesToMessage*";
    if (Addr == 0x08069841 || Addr == 0x0806985C) 						return "CNWSMessage::SendServerToPlayerChat_Shout*";
    if (Addr == 0x08069122 || Addr == 0x0806913D) 						return "CNWSMessage::SendServerToPlayerChat_Party";
    if (Addr == 0x080695EA || Addr == 0x08069605) 						return "CNWSMessage::SendServerToPlayerChat_Tell";

    if (Addr == 0x08206740 || Addr == 0x08206A30)						return "CNWVirtualMachineCommands::ExecuteCommandGetName#";
    if (Addr == 0x082305BA || Addr == 0x0823071E ||
            Addr == 0x08230992 || Addr == 0x08230AFD) 						return "CNWVirtualMachineCommands::ExecuteCommandGetDescription";
    if (Addr == 0x08075AC0) 											return "CNWSMessage::SendServerToPlayerDungeonMasterAreaList";
    if (Addr == 0x08163C65) 											return "CNWSCreatureStats::GetFullName";

    return "unknown";
}

unsigned char ***pESP;
int rEAX;

int GetNameHookProc(void *pObjectInfoStruct, int some_flag, char *objName_buf, int flag)
{
    asm("pusha");

    asm("movl 0x4(%ebp), %ebx");
    asm("movl  %ebx, pESP");
    int dbg = 0;
    if ((dword)pESP == 0x08052A0C || (dword)pESP == 0x08052941)
        dbg = 1;
    names.Log(4 + dbg, "GetName: Caller=%08lX, some_flag=%x, flag=%x\n", pESP, some_flag, flag);
//	else
//		names.Log(4, "GetName: Caller=%08lX, some_flag=%x, flag=%x -- SCRIPT: GetName()\n", pESP, some_flag, flag);
    //080751CC - read area name

    void *pPlayer = NULL;
    void *pCreature = NULL;
    void *pPlayerObj = NULL;
    dword nPlayerObjID = 0;
    dword nCreatureObjID = 0;
    dword nClientID = 0;
    bool bBroadcast = 0;

    char **pNewName;
    char *NewName;
    CPlayerNames *pPlayerEntry;


    int name_type;
    if ((dword)pESP == 0x080524FD) { //WriteCExoLocStringServer
        asm("movl (%ebp), %ebx");
        asm("movl %ebx, pESP");
        void *ReadCreatureName_ebp = pESP;

        dword ReadCreatureName_caller = *(dword *)((char *)ReadCreatureName_ebp + 0x4);
        names.Log(3, "--WriteCExoLocStringServer_caller = %08lX (%s)\n", ReadCreatureName_caller, GetServerFuncName(ReadCreatureName_caller));
        if (ReadCreatureName_caller == 0x0806A377 || ReadCreatureName_caller == 0x0806A38E) {
            //BuildCreatureMessage
            if (ReadCreatureName_caller == 0x0806A377) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x0806A38E) name_type = eLastName;

            void *BuildCreatureMessage_ebp = *pESP;
            //nCreatureObjID = *(dword *)((char *)BuildCreatureMessage_ebp+0x10);
            pCreature = *(dword **)((char *)BuildCreatureMessage_ebp + 0xC);
            void *BrainFuck2_ebp = **pESP;
            pPlayer = *(dword **)((char *)BrainFuck2_ebp + 0xC);

            if (!pPlayer) goto ext;
            pPlayerObj = GetPlayerObject(pPlayer);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            nCreatureObjID = *(dword *)((char *)pCreature + 0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08073401 || ReadCreatureName_caller == 0x08073418 || ReadCreatureName_caller == 0x080732AF || ReadCreatureName_caller == 0x080732C9) {
            //BrainFuck2
            if (ReadCreatureName_caller == 0x08073401 || ReadCreatureName_caller == 0x080732AF) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x08073418 || ReadCreatureName_caller == 0x080732C9) name_type = eLastName;

            void *BrainFuck2_ebp = *pESP;
            pCreature = *(dword **)((char *)BrainFuck2_ebp + 0x10);
            pPlayer = *(dword **)((char *)BrainFuck2_ebp + 0xC);

            if (!pPlayer) goto ext;
            pPlayerObj = GetPlayerObject(pPlayer);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            nCreatureObjID = *(dword *)((char *)pCreature + 0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08077301 || ReadCreatureName_caller == 0x08077314) {
            //Some_area_data
            if (ReadCreatureName_caller == 0x08077301) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x08077314) name_type = eLastName;

            void *SomeModAreaData_ebp = *pESP;
            nCreatureObjID = *(dword *)((char *)SomeModAreaData_ebp - 0x9C);
            pCreature = GetObjectByID(nCreatureObjID);
            pPlayer = *(dword **)((char *)SomeModAreaData_ebp + 0x10);
            nClientID = *(dword *)((char *)SomeModAreaData_ebp + 0xC);
            names.Log(3, "Message type: %08lX\n", nClientID);
            if (nClientID >= 0x0FFFFFFF5 && nClientID != 0x0FFFFFFF6) {
                bBroadcast = 1;
                nPlayerObjID = nCreatureObjID;
            } else {
                if (!pPlayer) goto ext;
                pPlayerObj = GetPlayerObject(pPlayer);

                if (!pPlayerObj || !pCreature) goto ext;
                nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            }
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08077827 || ReadCreatureName_caller == 0x08077838) {
            //Some_area_data2
            if (ReadCreatureName_caller == 0x08077827) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x08077838) name_type = eLastName;

            void *SomeModAreaData2_ebp = *pESP;
            nCreatureObjID = *(dword *)((char *)SomeModAreaData2_ebp - 0xA8);
            pCreature = GetObjectByID(nCreatureObjID);
            pPlayer = *(dword **)((char *)SomeModAreaData2_ebp + 0xC);
            nClientID = *(dword *)((char *)SomeModAreaData2_ebp + 0xC);
            names.Log(3, "Message type: %08lX\n", nClientID);
            if (nClientID >= 0x0FFFFFFF5 && nClientID != 0x0FFFFFFF6) {
                bBroadcast = 1;
                nPlayerObjID = nCreatureObjID;
            } else {
                if (!pPlayer) goto ext;
                pPlayerObj = GetPlayerObject(pPlayer);

                if (!pPlayerObj || !pCreature) goto ext;
                nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            }

            //nCreatureObjID = *(dword *)((char *)pCreature+0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x0807E517 || ReadCreatureName_caller == 0x0807E526) {
            //LoginCharList
            //SendServerToPlayerPlayModuleCharacterListResponse
            if (ReadCreatureName_caller == 0x0807E517) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x0807E526) name_type = eLastName;

            void *LoginCharList_ebp = *pESP;
            nCreatureObjID = *(dword *)((char *)LoginCharList_ebp + 0x10);
            pCreature = GetObjectByID(nCreatureObjID);
            nClientID = *(dword *)((char *)LoginCharList_ebp + 0xC);
            pPlayer = GetPlayerByClientID(nClientID);

            bBroadcast = 1;

            pPlayerObj = NULL;

            nPlayerObjID = nCreatureObjID;
            //nCreatureObjID = *(dword *)((char *)pCreature+0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08073BBE || ReadCreatureName_caller == 0x08073BDF) {
            //pcInput_ExamineCreature
            if (ReadCreatureName_caller == 0x08073BBE) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x08073BDF) name_type = eLastName;

            void *ExamineCreature_ebp = *pESP;
            nCreatureObjID = *(dword *)((char *)ExamineCreature_ebp + 0x10);
            pCreature = GetObjectByID(nCreatureObjID);
            pPlayer = *(dword **)((char *)ExamineCreature_ebp + 0xC);

            if (!pPlayer) goto ext;
            pPlayerObj = GetPlayerObject(pPlayer);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x0806C245 || ReadCreatureName_caller == 0x0806C563) {
            //CNWSMessage::CreateNewLastUpdateObject
            if (ReadCreatureName_caller == 0x0806C245) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x0806C563) name_type = eLastName;

            void *CreateNewLastUpdateObject_ebp = *pESP;
            pCreature = *(dword **)((char *)CreateNewLastUpdateObject_ebp + 0x10);
            pPlayer = *(dword **)((char *)CreateNewLastUpdateObject_ebp + 0xC);

            if (!pPlayer) goto ext;
            pPlayerObj = GetPlayerObject(pPlayer);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);
            nCreatureObjID = *(dword *)((char *)pCreature + 0x4);

            //nCreatureObjID = *(dword *)((char *)pCreature+0x4);
            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08069122 || ReadCreatureName_caller == 0x0806913D) {
            //CNWSMessage::SendServerToPlayerChat_Party
            if (ReadCreatureName_caller == 0x08069122) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x0806913D) name_type = eLastName;

            void *SendServerToPlayerChat_Party_ebp = *pESP;

            nClientID = *(dword *)((char *)SendServerToPlayerChat_Party_ebp + 0xC);
            nCreatureObjID = *(dword *)((char *)SendServerToPlayerChat_Party_ebp + 0x10);
            pPlayer = GetPlayerByClientID(nClientID);
            names.Log(3, "Message type: %08lX\n", nClientID);
            if (!pPlayer) goto ext;

            pPlayerObj = GetPlayerObject(pPlayer);
            pCreature = GetObjectByID(nCreatureObjID);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);

            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x080695EA || ReadCreatureName_caller == 0x08069605) {
            //CNWSMessage::SendServerToPlayerChat_Tell
            if (ReadCreatureName_caller == 0x080695EA) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x08069605) name_type = eLastName;

            void *SendServerToPlayerChat_Tell_ebp = *pESP;

            nClientID = *(dword *)((char *)SendServerToPlayerChat_Tell_ebp + 0xC);
            nCreatureObjID = *(dword *)((char *)SendServerToPlayerChat_Tell_ebp + 0x10);
            pPlayer = GetPlayerByClientID(nClientID);
            if (!pPlayer) goto ext;

            pPlayerObj = GetPlayerObject(pPlayer);
            pCreature = GetObjectByID(nCreatureObjID);

            if (!pPlayerObj || !pCreature) goto ext;
            nPlayerObjID = *(dword *)((char *)pPlayerObj + 0x4);

            names.Log(3, "Got data\n");
        } else if (ReadCreatureName_caller == 0x08069841 || ReadCreatureName_caller == 0x0806985C) {
            //CNWSMessage::SendServerToPlayerChat_Shout
            if (ReadCreatureName_caller == 0x08069841) name_type = eFirstName;
            else if (ReadCreatureName_caller == 0x0806985C) name_type = eLastName;

            void *SendServerToPlayerChat_Shout_ebp = *pESP;

            //nClientID = *(dword *)((char *)SendServerToPlayerChat_Shout_ebp+0xC);
            nCreatureObjID = *(dword *)((char *)SendServerToPlayerChat_Shout_ebp + 0x10);
            pCreature = GetObjectByID(nCreatureObjID);
            pPlayer = NULL;
            bBroadcast = 1;

            pPlayerObj = NULL;

            nPlayerObjID = nCreatureObjID;
            //pPlayer = GetPlayerByClientID(nClientID);
            //names.Log(3, "Message type: %08lX\n", nClientID);
            /*if(nClientID >= 0x0FFFFFFF5)
            {
            	bBroadcast=1;
            	pCreature = GetObjectByID(nCreatureObjID);
            }
            else
            {
            	if(!pPlayer) goto ext;
            	pPlayerObj = GetPlayerObject(pPlayer);
            	pCreature = GetObjectByID(nCreatureObjID);

            	if(!pPlayerObj || !pCreature) goto ext;
            	nPlayerObjID = *(dword *)((char *)pPlayerObj+0x4);
            }*/

            names.Log(3, "Got data\n");
        }




        /*		else if(ReadCreatureName_caller==0x08069366 || ReadCreatureName_caller==0x08069381)
        		{
        			//chat_ReadObjName5Private
        			if(ReadCreatureName_caller==0x08069366) name_type = eFirstName;
        			else if(ReadCreatureName_caller==0x08069381) name_type = eLastName;

        			void *ReadObjName5Private_ebp = *pESP;
        			nCreatureObjID = *(dword *)((char *)ReadObjName5Private_ebp+0x10);
        			pCreature = RetObjByOID(nCreatureObjID);
        			pPlayer = *(dword **)((char *)ReadObjName5Private_ebp+0xC);

        			if(!pPlayer) goto ext;
        			pPlayerObj = GetPlayerObject(pPlayer);

        			if(!pPlayerObj || !pCreature) goto ext;
        			nPlayerObjID = *(dword *)((char *)pPlayerObj+0x4);
        			names.Log(3, "Got data\n");
        		}*/
        /*else if(ReadCreatureName_caller==0x08069366 || ReadCreatureName_caller==0x08069381)
        {
        	if(ReadCreatureName_caller==0x08069366) name_type = eFirstName;
        	else if(ReadCreatureName_caller==0x08069381) name_type = eLastName;

        	void *ChatReadObjName5Private_ebp = *pESP;
        	pCreature = *(dword **)((char *)ChatReadObjName5Private_ebp+0xC);

        }*/

        else goto ext;
    } else if ((dword)pESP == 0x08052A0C || (dword)pESP == 0x08052941) { //GetLocStringServer
        asm("movl (%ebp), %ebx");
        asm("movl %ebx, pESP");
        void *GetLocStringServer_ebp = pESP;
        /*void *pPlayer = NULL;
        void *pCreature = NULL;
        void *pPlayerObj = NULL;
        dword nPlayerObjID = 0;
        dword nCreatureObjID = 0;
        bool bBroadcast = 0;*/

        dword GetLocStringServer_caller = *(dword *)((char *)GetLocStringServer_ebp + 0x4);
        if (GetLocStringServer_caller == 0x08206740 || GetLocStringServer_caller == 0x08206A30) {
            names.Log(4 + dbg, "--GetLocStringServer_caller = %08lX (%s)\n", GetLocStringServer_caller, GetServerFuncName(GetLocStringServer_caller));
            goto ext;
        } else if (GetLocStringServer_caller == 0x08163C65) { //GetFullName
            dbg = 0;
            names.Log(4 + dbg, "--GetLocStringServer_caller = %08lX (%s)\n", GetLocStringServer_caller, GetServerFuncName(GetLocStringServer_caller));
            void *GetFullName_ebp = *pESP;
            dword GetFullName_caller = *(dword *)((char *)GetFullName_ebp + 0x4);
            if (GetFullName_caller == 0x08197EA0) { //HandlePlayerToServerParty
                void *HandlePlayerToServerParty_ebp = **pESP;
                nPlayerObjID = *(dword *)((char *)HandlePlayerToServerParty_ebp - 0x88);
                void *pInviter = *(dword **)((char *)HandlePlayerToServerParty_ebp + 0xC);
                pCreature = GetPlayerObject(pInviter);
                nCreatureObjID = *(dword *)((char *)pCreature + 0x4);
                names.Log(3, "Got data\n");
            }
        } else {
            dbg = 0;
            names.Log(4 + dbg, "--GetLocStringServer_caller = %08lX (%s)\n", GetLocStringServer_caller, GetServerFuncName(GetLocStringServer_caller));
            goto ext;
        }
        //names.Log(4+dbg, "--GetLocStringServer_caller = %08lX (%s)\n", GetLocStringServer_caller, GetServerFuncName(GetLocStringServer_caller));
    }

    if (!pPlayerObj)
        pPlayerObj = GetObjectByID(nPlayerObjID);


    if (pPlayerObj) {
        CNWSCreature *pCr = (CNWSCreature *)pPlayerObj;
        if (pCr->MasterID != OBJECT_INVALID) {
            void *pCreatureTmp = GetObjectByID(pCr->MasterID);
            if (pCreatureTmp) {
                nPlayerObjID = pCr->MasterID;
            }
        }
    }

    pNewName = new char *;
    names.Log(3, "Finding custom name for %08lX-%08lX... ", nPlayerObjID, nCreatureObjID);
    pPlayerEntry = names.Names.FindPlayerEntry(nPlayerObjID);
    if (!pPlayerEntry || !pPlayerEntry->bEnabled) goto ext;
    NewName = names.Names.FindCustomName(nPlayerObjID, nCreatureObjID);
    names.Log(3, "Done.\n");
    if (!NewName || bBroadcast) {
        names.Log(3, "No new name\n");
        if (((nPlayerObjID != nCreatureObjID) || bBroadcast) && GetIsPC(nCreatureObjID) && !GetIsDM(nCreatureObjID)) {
            names.Log(3, "IsPC\n");
            int nStyle;
            pPlayerEntry = names.Names.FindPlayerEntry(nPlayerObjID);
            if (!pPlayerEntry) nStyle = 1;
            else {
                nStyle = pPlayerEntry->UnknownStyle;
            }
            if (nStyle == 0) {
                NewName = "";
            }
            if (nStyle == 1) {
                char *nCreatureStruct = *(char **)((char *)pCreature + 0xC68);
                char nCreatureGender = *((char *)nCreatureStruct + 0x6C);
                if (nCreatureGender == 1) NewName = "<c–––>Íåçíàêîìêà</c>";
                else NewName = "<c–––>Íåçíàêîìåö</c>";
            } else if (nStyle == 2) {
                NewName = "<c–––>Unknown</c>";
            } else {
                NewName = "ÃËÞÊ!!!";
            }
        } else {
            goto ext2;
        }
    }
    //char *NewName = new char[25];
    //sprintf(NewName, "%08lX - %08lX", nPlayerObjID, nCreatureObjID);
    if (name_type == eFirstName) *pNewName = NewName;
    else *pNewName = "";
    d_newstrcpy(objName_buf, pNewName);
    delete pNewName;
    names.Log(4, "--Return value: %s\n", *(char **)objName_buf);
    return 1;

ext2:
    delete pNewName;
    goto ext;




ext:
    asm("popa");
    //asm ("leave");
    asm("mov 0x14(%ebp), %eax");
    asm("push %eax");
    asm("mov 0x10(%ebp), %eax");
    asm("push %eax");
    asm("mov 0xC(%ebp), %eax");
    asm("push %eax");
    asm("mov 0x8(%ebp), %eax");
    asm("push %eax");
    asm("mov $d_ret_code_nm, %eax");
    //asm ("jmp %eax");
    asm("call *%eax");
    asm("add $0x10, %esp");

    asm("pusha");
    //if((dword)pESP!=0x08052A04 && (dword)pESP!=0x08052939)
    names.Log(4 + dbg, "--Return value: %s\n", *(char **)objName_buf);
    asm("popa");
    asm("leave");
    asm("ret");
    return 1;
}

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

dword nPlayerListAdd_PlayerId = -1;

bool CNWSMessage__SendServerToPlayerPlayerList_Add_Hook(CNWSMessage *pMessage, dword nPlayerId, CNWSPlayer *pNewPlayer)
{
    nPlayerListAdd_PlayerId = nPlayerId;
    return CNWSMessage__SendServerToPlayerPlayerList_Add(pMessage, nPlayerId, pNewPlayer);
}

void SendPlayerListAdd_WritePlayerName(CNWSMessage* msg, CExoString &value)
{
    names.Log(5, "SendPlayerListAdd_WritePlayerName: player %s, playerId %d\n", value.Text, nPlayerListAdd_PlayerId);
    if (nPlayerListAdd_PlayerId != PLAYERID_ALL_GAMEMASTERS) {
        char maskedPlayerName[17];
        rand_str(maskedPlayerName, 16);
        names.Log(5, "SendPlayerListAdd_WritePlayerName: replaced with %s\n", maskedPlayerName);
        msg->WriteCExoString(CExoString(maskedPlayerName), 32);
    } else {
        msg->WriteCExoString(value, 32);
    }
}

CNWSPlayer *pPlayerListAll_Player = NULL;

bool CNWSMessage__SendServerToPlayerPlayerList_All_Hook(CNWSMessage *pMessage, CNWSPlayer *pPlayer)
{
    pPlayerListAll_Player = pPlayer;
    return CNWSMessage__SendServerToPlayerPlayerList_All(pMessage, pPlayer);
}

void SendPlayerListAll_WritePlayerName(CNWSMessage* msg, CExoString &value)
{
    names.Log(5, "SendPlayerListAll_WritePlayerName: player %s, playerId: %d\n", value.Text, pPlayerListAll_Player->m_nPlayerID);
    CNetLayerPlayerInfo *playerInfo = g_pAppManager->ServerExoApp->GetNetLayer()->GetPlayerInfo(pPlayerListAll_Player->m_nPlayerID);
    if (playerInfo != NULL && !playerInfo->IsDM) {
        char maskedPlayerName[17];
        rand_str(maskedPlayerName, 16);
        names.Log(5, "SendPlayerListAll_WritePlayerName: replaced with %s\n", maskedPlayerName);
        msg->WriteCExoString(CExoString(maskedPlayerName), 32);
    } else {
        msg->WriteCExoString(value, 32);
    }
}

void
d_enable_write(unsigned long location)
{
    char *page;
    page = (char *) location;
    page = (char *)(((int) page + PAGESIZE - 1) & ~(PAGESIZE - 1));
    page -= PAGESIZE;

    if (mprotect(page, PAGESIZE, PROT_WRITE | PROT_READ | PROT_EXEC))
        perror("mprotect");
}

int intlen = -1;

void
d_redirect(long from, long to, unsigned char *d_ret_code, long len = 0)
{
    // enable write to code pages
    d_enable_write(from);
    // copy orig code stub to our "ret_code"
    len = len ? len : sizeof(d_jmp_code) - 1; // - trailing 0x00
    intlen = len;
    memcpy((void *) d_ret_code, (const void *) from, len);
    // make ret code
    *(long *)(d_jmp_code + 1) = from + len;
    memcpy((char *) d_ret_code + len, (const void *) d_jmp_code, 6);
    // make hook code
    *(long *)(d_jmp_code + 1) = to;
    memcpy((void *) from, (const void *) d_jmp_code, 6);
}

void InitConstants()
{
    *(dword*)&pServer = *ppServer; //CAppManager
    *(dword*)&pServerExo = *(dword*)((char*)pServer + 0x4); //CServerExoApp
    *(dword*)&pServerExo4 = *(dword*)((char*)pServerExo + 0x4); //CServerExoAppInternal

    *(dword*)&pObjectClass = *(dword*)(*(dword*)((char*)pServerExo + 0x4) + 0x10080);
    *(dword*)&pFactionClass = *(dword*)(*(dword*)((char*)pServerExo + 0x4) + 0x10074);
    *(dword*)&pClientClass = *(dword*)(*(dword*)((char*)pServerExo + 0x4) + 0x10060);
}

int HookFunctions()
{
    dword org_GetName = 0x082CA118;  //CExoLocString::GetStringLoc(int, CExoString *, unsigned char)
    *(dword*)&d_newstrcpy = 0x082CAD34;  //CExoString::__as(CExoString const &)
    *(dword*)&GetPlayerObject = 0x0805E8B8;  //CNWSPlayer::GetGameObject(void)
    *(dword*)&pGetPlayerByOID = 0x080B24B8;  //CServerExoApp::GetClientObjectByObjectId(unsigned long)
    *(dword*)&pGetPlayerList = 0x080B1F2C;  //CServerExoApp::GetPlayerList(void)
    *(dword*)&pRetObjByOID = 0x080B1DE0;  //CServerExoApp::GetCreatureByGameObjectID(unsigned long)
    *(dword*)&pGetServerMessage = 0x080B1F54;  //CServerExoApp::GetNWSMessage(void)
    *(dword*)&pGetObjByOID = 0x080BB2CC;
    *(dword*)&GetClientObjectByPlayerId = 0x080B24D0;

    *(dword*)&CNWMessage__CreateWriteMessage = 0x080C3AEC;
    *(dword*)&CNWSMessage__WriteGameObjUpdate_UpdateObject = 0x08071A24;
    *(dword*)&CNWMessage__GetWriteMessage = 0x080C2E54;
    *(dword*)&CNWSMessage__SendServerToPlayerMessage = 0x08076F10;
    *(dword*)&CNWSMessage__SendServerToPlayerPlayerList_All = 0x080774E4;

    ppServer = (dword *) 0x0832F1F4;  //CAppManager *g_pAppManager

    *(dword*)&pScriptThis = (dword)((char*)ppServer - 0x8);

    d_redirect(org_GetName, (unsigned long)GetNameHookProc, d_ret_code_nm, 13);

    NX_HOOK(CNWSMessage__SendServerToPlayerPlayerList_Add, 0x080770A8, CNWSMessage__SendServerToPlayerPlayerList_Add_Hook, 5);
    NX_HOOK_CALL(0x080772B8, (long)SendPlayerListAdd_WritePlayerName);
    NX_HOOK(CNWSMessage__SendServerToPlayerPlayerList_All, 0x080774E4, CNWSMessage__SendServerToPlayerPlayerList_All_Hook, 5);
    NX_HOOK_CALL(0x080777E0, (long)SendPlayerListAll_WritePlayerName);

    return 1;
}


#include "nwnx_auth"

int StartingConditional()
{
    string sAccount = NWNXAuth_GetAccount();
    string sCDKey = NWNXAuth_GetCDKey();
    WriteTimestampedLogEntry("onAuth: " + sAccount + "(" + sCDKey + ")");
    return Random(2);
}
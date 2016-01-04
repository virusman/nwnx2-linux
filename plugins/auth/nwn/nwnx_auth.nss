// Get the account name of the user trying to connect
// Only works in on_auth script
string NWNXAuth_GetAccount();

// Get the public CD-key of the user trying to connect
// Only works in on_auth script
string NWNXAuth_GetCDKey();



string NWNXAuth_GetAccount()
{
    SetLocalString(GetModule(), "NWNX!AUTH!GET_ACCOUNT", "          ");
    return GetLocalString(GetModule(), "NWNX!AUTH!GET_ACCOUNT");
}

string NWNXAuth_GetCDKey()
{
    SetLocalString(GetModule(), "NWNX!AUTH!GET_CDKEY", "          ");
    return GetLocalString(GetModule(), "NWNX!AUTH!GET_CDKEY");
}
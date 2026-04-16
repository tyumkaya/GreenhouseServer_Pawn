DialogCreate:RegisterMainPass(playerid)
{
    static const fmt_string[] = "Welcome, %s, to a "#C_GREEN""SERVER_NAME""#C_WHITE" test server\n\
    Proceed to Registration first\n\
    Create a password for your account and press \"Continue\"\n\n\
    Tip: \n\
    \tThe password length should be between 6 and 20 characters.\n\
    \tThe password must contain both letters and numbers.\n\
    \tThe password is case-sensitive";

    new string[sizeof(fmt_string) + MAX_PLAYER_NAME - 2];

    format(string, sizeof(string), fmt_string, GetPlayer(playerid, pName));

    return Dialog_Open(playerid, Dialog:RegisterMainPass, DIALOG_STYLE_PASSWORD, "Registration | Enter Password",
    string, "Continue", "Exit");
}

DialogCreate:LoginToServer(playerid)
{
    static const fmt_string[] = "\
    Welcome back, %s, to a "#C_GREEN""SERVER_NAME""#C_WHITE" test server\n\
    Please enter a password and press \"Continue\"";

    new string[sizeof(fmt_string) + MAX_PLAYER_NAME - 2];

    format(string, sizeof(string), fmt_string, GetPlayer(playerid, pName));

    return Dialog_Open(playerid, Dialog:LoginToServer, DIALOG_STYLE_PASSWORD, "Authorization | Enter Password",
    string, "Continue", "Exit");
}

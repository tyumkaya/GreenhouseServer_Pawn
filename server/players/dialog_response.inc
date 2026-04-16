DialogR:RegisterMainPass()
{
    if(!response)
    {
        SendClientMessage(playerid, -1, "You left the server. \"/q\"");
        return Kick(playerid);
    } else
    {
        new len = strlen(inputtext);
        if(!len)
        {
            SendClientMessage(playerid, -1, "Enter Password into registration field.");
            return Dialog_Show(playerid, Dialog:RegisterMainPass);
        }
        if(len < 6 || len > 20)
        {
            SendClientMessage(playerid, -1, "The password length should be between 6 and 20 characters.");
            return Dialog_Show(playerid, Dialog:RegisterMainPass);
        }
        new Regex:registerMainPassword = Regex_New("^[a-zA-Z0-9]{6,20}");
        if(Regex_Check(inputtext, registerMainPassword))
        {
            strmid(GetPlayer(playerid, pPassword), inputtext, 0, len, 20);
            Regex_Delete(registerMainPassword);
            return RegisterPlayer(playerid);

        } else
        {
            SendClientMessage(playerid, -1, "Only latin letters and numbers.");
            Dialog_Show(playerid, Dialog:registerMainPassword);
            return Regex_Delete(registerMainPassword);
            
        }
    }
}

DialogR:LoginToServer()
{
    if(!response)
    {
        SendClientMessage(playerid, -1, "You left the server. \"/q\"");
        return Kick(playerid);
    } else
    {
        new len = strlen(inputtext);
        if(!len)
        {
            SendClientMessage(playerid, -1, "Enter Password into registration field.");
            return Dialog_Show(playerid, Dialog:LoginToServer);
        }
        
        static const fmt_query[] = "SELECT `id`, `password` FROM `users` WHERE `name` = '%s' AND `password` = '%s' LIMIT 1";
        new query[sizeof(fmt_query) + MAX_PLAYER_NAME + 20 - 4];
        mysql_format(dbHandle, query, sizeof(query), fmt_query, GetPlayer(playerid, pName), inputtext);
        return mysql_tquery(dbHandle, query, "CheckLoginToServer", "i", playerid);
    }
}

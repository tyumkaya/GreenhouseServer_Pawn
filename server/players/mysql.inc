public: CheckPlayerToBase(playerid)
{
    new rows;
    cache_get_row_count(rows);
    if(!rows) {
        return Dialog_Show(playerid, Dialog:RegisterMainPass);
    } else {
        return Dialog_Show(playerid, Dialog:LoginToServer);

    }
}

public: CheckLoginToServer(playerid)
{
    new rows;
    cache_get_row_count(rows);
    if(!rows) {
        SendClientMessage(playerid, -1, "Wrong Password");
        return Dialog_Show(playerid, Dialog:LoginToServer);
    } else {
        cache_get_value_int(0, "id", GetPlayer(playerid, pID));
        SendClientMessage(playerid, -1, "Successfully logged in");
        return BeginPlayerAuthorization(playerid);
    }
}

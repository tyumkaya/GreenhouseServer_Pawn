stock RegisterPlayer(playerid)
{
    static const ftm_query[] = "INSERT INTO `users` (`name`, `password`) VALUES ('%s', '%s')";
    new query[sizeof(ftm_query) + MAX_PLAYER_NAME + 20];
    mysql_format(dbHandle, query, sizeof(query), ftm_query, GetPlayer(playerid, pName), GetPlayer(playerid, pPassword));
    return mysql_tquery(dbHandle, query, "FinishPlayerRegistration", "i", playerid);
}

public: FinishPlayerRegistration(playerid)
{
    GetPlayer(playerid, pID) = cache_insert_id();
    SendClientMessage(playerid, -1, "Successfully registered");
    return BeginPlayerAuthorization(playerid);
}

stock BeginPlayerAuthorization(playerid)
{
    GetPlayer(playerid, pLogin) = true;
    return LoadPlayerGreenhouses(playerid);
}

stock CompletePlayerAuthorization(playerid)
{
    return SpawnGamer(playerid);
}

stock ResetPlayerAccountData(playerid)
{
    GetPlayer(playerid, pID) = 0;
    GetPlayer(playerid, pPassword)[0] = EOS;
    GetPlayer(playerid, pLogin) = false;
    return 1;
}

stock SpawnGamer(playerid) 
{
    SetSpawnInfo(playerid, 244, 0, 112.938, -82.637, 1.57812, 90, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    return SpawnPlayer(playerid);
}

stock LoadPlayerGreenhouses(playerid)
{
    if(GetPlayer(playerid, pID) <= 0)
    {
        return CompletePlayerAuthorization(playerid);
    }

    ResetPlayerGreenhouses(playerid);

    static const fmt_query[] = "\
    SELECT `id`, `owner_id`, `slot`, `plant_type`, `upgrade_level`, `growth_multiplier`, `state`, `stage`, `growth_seconds`, `mature_at_growth_seconds`, `pos_x`, `pos_y`, `pos_z`, `interior`, `world` \
    FROM `greenhouses` WHERE `owner_id` = %d ORDER BY `slot` ASC LIMIT 5";

    new query[sizeof(fmt_query) + 16];
    mysql_format(dbHandle, query, sizeof(query), fmt_query, GetPlayer(playerid, pID));
    return mysql_tquery(dbHandle, query, "LoadPlayerGreenhousesCallback", "i", playerid);
}

stock SaveAndUnloadPlayerGreenhouses(playerid)
{
    SavePlayerGreenhouses(playerid);
    return UnloadPlayerGreenhouses(playerid);
}

stock SaveAllGreenhouses()
{
    for(new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if(!IsPlayerConnected(playerid) || !GetPlayerGreenhouse(playerid, pGhLoaded))
        {
            continue;
        }

        SavePlayerGreenhouses(playerid);
    }
    return 1;
}

public: ProcessGreenhouses()
{
    greenhouseUpdateTick++;
    for(new greenhouseid = 0; greenhouseid < MAX_GREENHOUSES; greenhouseid++)
    {
        if(!GetGreenhouse(greenhouseid, ghExists))
        {
            continue;
        }
        if(GetGreenhouse(greenhouseid, ghState) == GH_STATE_EMPTY || GetGreenhouse(greenhouseid, ghState) == GH_STATE_MATURE)
        {
            continue;
        }
        if(!IsGreenhouseOwnerOnline(greenhouseid))
        {
            continue;
        }

        ProcessGreenhouseGrowth(greenhouseid);
    }

    if(greenhouseUpdateTick >= 60)
    {
        greenhouseUpdateTick = 0;
        SaveAllGreenhouses();
    }
    return 1;
}

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
    new greenhouseid = FindGreenhouseByArea(areaid);
    new string[160];

    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return 1;
    }
    if(GetGreenhouse(greenhouseid, ghOwnerID) != GetPlayer(playerid, pID))
    {
        return 1;
    }

    GetPlayerGreenhouse(playerid, pGhLastNearbyID) = greenhouseid;
    GetGreenhouseStatusText(greenhouseid, string, sizeof(string));
    SendClientMessage(playerid, -1, string);
    SendClientMessage(playerid, -1, "Nearby interaction: /ghstatus /ghplant /ghupgrade /ghharvest");
    return 1;
}

public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
    new greenhouseid = GetPlayerGreenhouse(playerid, pGhLastNearbyID);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return 1;
    }
    if(GetGreenhouse(greenhouseid, ghAreaID) == areaid)
    {
        GetPlayerGreenhouse(playerid, pGhLastNearbyID) = INVALID_GREENHOUSE_ID;
    }
    return 1;
}

CMD:ghcreate(playerid)
{
    new greenhouseid;
    if(!IsPlayerAdmin(playerid))
    {
        return SendClientMessage(playerid, -1, "Admin command.");
    }

    greenhouseid = CreatePlayerGreenhouse(playerid);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return 1;
    }

    new string[96];
    format(string, sizeof(string), "Greenhouse created. Slot: %d, SQL ID: %d.", GetGreenhouse(greenhouseid, ghSlot) + 1, GetGreenhouse(greenhouseid, ghSQLID));
    return SendClientMessage(playerid, -1, string);
}

CMD:ghlist(playerid)
{
    new string[144];
    SendClientMessage(playerid, -1, "Your greenhouses:");
    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        new greenhouseid = GetPlayerGreenhouseBySlot(playerid, slot);
        if(greenhouseid == INVALID_GREENHOUSE_ID)
        {
            continue;
        }

        format(string, sizeof(string), "Slot %d | SQL %d | State %d | Stage %d | Growth %d/%d | x%d",
            slot + 1,
            GetGreenhouse(greenhouseid, ghSQLID),
            _:GetGreenhouse(greenhouseid, ghState),
            GetGreenhouse(greenhouseid, ghStage),
            GetGreenhouse(greenhouseid, ghGrowthSeconds),
            GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds),
            GetGreenhouse(greenhouseid, ghGrowthMultiplier)
        );
        SendClientMessage(playerid, -1, string);
    }
    return 1;
}

CMD:ghplant(playerid)
{
    new greenhouseid = FindNearestPlayerGreenhouse(playerid);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Move closer to one of your greenhouses.");
    }
    return PlantTomatoInGreenhouse(playerid, greenhouseid);
}

CMD:ghupgrade(playerid)
{
    new greenhouseid = FindNearestPlayerGreenhouse(playerid);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Move closer to one of your greenhouses.");
    }
    return UpgradeGreenhouse(playerid, greenhouseid);
}

CMD:ghharvest(playerid)
{
    new greenhouseid = FindNearestPlayerGreenhouse(playerid);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Move closer to one of your greenhouses.");
    }
    return HarvestGreenhouse(playerid, greenhouseid);
}

CMD:ghstatus(playerid)
{
    new greenhouseid = FindNearestPlayerGreenhouse(playerid);
    new string[144];
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Move closer to one of your greenhouses.");
    }

    format(string, sizeof(string), "Greenhouse SQL %d | State %d | Growth %d%% | Stage %d | x%d",
        GetGreenhouse(greenhouseid, ghSQLID),
        _:GetGreenhouse(greenhouseid, ghState),
        GetGrowthPercent(greenhouseid),
        GetGreenhouse(greenhouseid, ghStage),
        GetGreenhouse(greenhouseid, ghGrowthMultiplier)
    );
    return SendClientMessage(playerid, -1, string);
}

CMD:ghreset(playerid)
{
    new greenhouseid;
    if(!IsPlayerAdmin(playerid))
    {
        return SendClientMessage(playerid, -1, "Admin command.");
    }
    greenhouseid = FindNearestPlayerGreenhouse(playerid, 20.0);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "No greenhouse nearby.");
    }
    return ResetGreenhouseForTest(playerid, greenhouseid);
}

CMD:ghforce(playerid)
{
    new greenhouseid;
    if(!IsPlayerAdmin(playerid))
    {
        return SendClientMessage(playerid, -1, "Admin command.");
    }
    greenhouseid = FindNearestPlayerGreenhouse(playerid, 20.0);
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "No greenhouse nearby.");
    }
    return ForceGreenhouseMature(playerid, greenhouseid);
}

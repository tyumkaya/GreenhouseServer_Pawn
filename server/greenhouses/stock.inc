stock ResetGreenhouse(greenhouseid)
{
    GetGreenhouse(greenhouseid, ghExists) = false;
    GetGreenhouse(greenhouseid, ghSQLID) = 0;
    GetGreenhouse(greenhouseid, ghOwnerID) = 0;
    GetGreenhouse(greenhouseid, ghSlot) = 0;
    GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_NONE;
    GetGreenhouse(greenhouseid, ghUpgradeLevel) = 0;
    GetGreenhouse(greenhouseid, ghGrowthMultiplier) = 1;
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_EMPTY;
    GetGreenhouse(greenhouseid, ghStage) = 0;
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = 0;
    GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds) = 600;
    GetGreenhouse(greenhouseid, ghPosX) = 0.0;
    GetGreenhouse(greenhouseid, ghPosY) = 0.0;
    GetGreenhouse(greenhouseid, ghPosZ) = 0.0;
    GetGreenhouse(greenhouseid, ghInterior) = 0;
    GetGreenhouse(greenhouseid, ghWorld) = 0;
    GetGreenhouse(greenhouseid, ghObjectID) = INVALID_STREAMER_ID;
    GetGreenhouse(greenhouseid, ghLabelID) = INVALID_STREAMER_ID;
    GetGreenhouse(greenhouseid, ghAreaID) = INVALID_STREAMER_ID;
    GetGreenhouse(greenhouseid, ghBusy) = false;
    GetGreenhouse(greenhouseid, ghDirtyFlags) = GH_DIRTY_NONE;
    return 1;
}

stock ResetPlayerGreenhouses(playerid)
{
    GetPlayerGreenhouse(playerid, pGhLoaded) = false;
    GetPlayerGreenhouse(playerid, pGhCount) = 0;
    GetPlayerGreenhouse(playerid, pGhLastNearbyID) = INVALID_GREENHOUSE_ID;

    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        PlayerGreenhouseData[playerid][pGhIDs][slot] = INVALID_GREENHOUSE_ID;
    }
    return 1;
}

stock GetGreenhouseStageByGrowth(greenhouseid)
{
    new matureAt = GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds);
    new growth = GetGreenhouse(greenhouseid, ghGrowthSeconds);
    if(growth <= 0)
    {
        return 0;
    }
    if(growth >= matureAt)
    {
        return 3;
    }

    new percent = (growth * 100) / matureAt;
    if(percent >= 67)
    {
        return 2;
    }
    if(percent >= 34)
    {
        return 1;
    }
    return 0;
}

stock MarkGreenhouseDirty(greenhouseid, dirtyFlags)
{
    GetGreenhouse(greenhouseid, ghDirtyFlags) |= dirtyFlags;
    return 1;
}

stock GetPlayerGreenhouseBySlot(playerid, slot)
{
    if(slot < 0 || slot >= MAX_PLAYER_GREENHOUSES)
    {
        return INVALID_GREENHOUSE_ID;
    }
    return PlayerGreenhouseData[playerid][pGhIDs][slot];
}

stock FindFreeGreenhouseSlot()
{
    for(new greenhouseid = 0; greenhouseid < MAX_GREENHOUSES; greenhouseid++)
    {
        if(!GetGreenhouse(greenhouseid, ghExists))
        {
            ResetGreenhouse(greenhouseid);
            return greenhouseid;
        }
    }
    return INVALID_GREENHOUSE_ID;
}

stock AttachGreenhouseToPlayer(playerid, greenhouseid)
{
    new slot = GetGreenhouse(greenhouseid, ghSlot);
    if(slot < 0 || slot >= MAX_PLAYER_GREENHOUSES)
    {
        slot = GetPlayerGreenhouse(playerid, pGhCount);
        if(slot >= MAX_PLAYER_GREENHOUSES)
        {
            return 0;
        }
        GetGreenhouse(greenhouseid, ghSlot) = slot;
    }

    if(PlayerGreenhouseData[playerid][pGhIDs][slot] == INVALID_GREENHOUSE_ID)
    {
        GetPlayerGreenhouse(playerid, pGhCount)++;
    }
    PlayerGreenhouseData[playerid][pGhIDs][slot] = greenhouseid;
    return 1;
}

stock SaveGreenhouse(greenhouseid)
{
    if(greenhouseid == INVALID_GREENHOUSE_ID || !GetGreenhouse(greenhouseid, ghExists))
    {
        return 0;
    }
    if(GetGreenhouse(greenhouseid, ghDirtyFlags) == GH_DIRTY_NONE)
    {
        return 0;
    }

    static const fmt_query[] = "\
    UPDATE `greenhouses` SET \
    `owner_id` = %d, `slot` = %d, `plant_type` = %d, `upgrade_level` = %d, `growth_multiplier` = %d, \
    `state` = %d, `stage` = %d, `growth_seconds` = %d, `mature_at_growth_seconds` = %d, \
    `pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `interior` = %d, `world` = %d \
    WHERE `id` = %d LIMIT 1";

    new query[sizeof(fmt_query) + 160];
    mysql_format(dbHandle, query, sizeof(query), fmt_query,
        GetGreenhouse(greenhouseid, ghOwnerID),
        GetGreenhouse(greenhouseid, ghSlot),
        _:GetGreenhouse(greenhouseid, ghPlantType),
        GetGreenhouse(greenhouseid, ghUpgradeLevel),
        GetGreenhouse(greenhouseid, ghGrowthMultiplier),
        _:GetGreenhouse(greenhouseid, ghState),
        GetGreenhouse(greenhouseid, ghStage),
        GetGreenhouse(greenhouseid, ghGrowthSeconds),
        GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds),
        GetGreenhouse(greenhouseid, ghPosX),
        GetGreenhouse(greenhouseid, ghPosY),
        GetGreenhouse(greenhouseid, ghPosZ),
        GetGreenhouse(greenhouseid, ghInterior),
        GetGreenhouse(greenhouseid, ghWorld),
        GetGreenhouse(greenhouseid, ghSQLID)
    );
    mysql_query(dbHandle, query, false);
    GetGreenhouse(greenhouseid, ghDirtyFlags) = GH_DIRTY_NONE;
    return 1;
}

stock FindFreePlayerGreenhouseSlot(playerid)
{
    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        if(PlayerGreenhouseData[playerid][pGhIDs][slot] == INVALID_GREENHOUSE_ID)
        {
            return slot;
        }
    }
    return INVALID_GREENHOUSE_ID;
}

stock IsGreenhouseOwnerOnline(greenhouseid)
{
    new ownerID = GetGreenhouse(greenhouseid, ghOwnerID);
    if(ownerID <= 0)
    {
        return false;
    }

    for(new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if(!IsPlayerConnected(playerid))
        {
            continue;
        }
        if(!GetPlayer(playerid, pLogin))
        {
            continue;
        }
        if(GetPlayer(playerid, pID) == ownerID)
        {
            return true;
        }
    }
    return false;
}

stock GetGreenhouseOwnerPlayer(greenhouseid)
{
    new ownerID = GetGreenhouse(greenhouseid, ghOwnerID);
    if(ownerID <= 0)
    {
        return INVALID_PLAYER_ID;
    }

    for(new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if(!IsPlayerConnected(playerid) || !GetPlayer(playerid, pLogin))
        {
            continue;
        }
        if(GetPlayer(playerid, pID) == ownerID)
        {
            return playerid;
        }
    }
    return INVALID_PLAYER_ID;
}

stock ProcessGreenhouseGrowth(greenhouseid)
{
    new growthStep = GetGreenhouse(greenhouseid, ghGrowthMultiplier);
    new matureAt = GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds);
    new oldStage = GetGreenhouse(greenhouseid, ghStage);

    if(growthStep < 1)
    {
        growthStep = 1;
    }

    GetGreenhouse(greenhouseid, ghGrowthSeconds) += growthStep;
    if(GetGreenhouse(greenhouseid, ghGrowthSeconds) >= matureAt)
    {
        GetGreenhouse(greenhouseid, ghGrowthSeconds) = matureAt;
        GetGreenhouse(greenhouseid, ghState) = GH_STATE_MATURE;
    }
    else
    {
        GetGreenhouse(greenhouseid, ghState) = GH_STATE_GROWING;
    }

    GetGreenhouse(greenhouseid, ghStage) = GetGreenhouseStageByGrowth(greenhouseid);
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_GROWTH);

    if(GetGreenhouse(greenhouseid, ghStage) != oldStage || GetGreenhouse(greenhouseid, ghState) == GH_STATE_MATURE)
    {
        MarkGreenhouseDirty(greenhouseid, GH_DIRTY_STATE);
        RefreshGreenhouseVisuals(greenhouseid);
    }
    return 1;
}

stock IsPlayerNearOwnGreenhouse(playerid, greenhouseid, Float:distance = 3.0)
{
    if(greenhouseid == INVALID_GREENHOUSE_ID || !GetGreenhouse(greenhouseid, ghExists))
    {
        return false;
    }
    if(GetGreenhouse(greenhouseid, ghOwnerID) != GetPlayer(playerid, pID))
    {
        return false;
    }
    if(GetPlayerInterior(playerid) != GetGreenhouse(greenhouseid, ghInterior))
    {
        return false;
    }
    if(GetPlayerVirtualWorld(playerid) != GetGreenhouse(greenhouseid, ghWorld))
    {
        return false;
    }

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    return GetPlayerDistanceFromPoint(playerid, GetGreenhouse(greenhouseid, ghPosX), GetGreenhouse(greenhouseid, ghPosY), GetGreenhouse(greenhouseid, ghPosZ)) <= distance;
}

stock FindNearestPlayerGreenhouse(playerid, Float:maxDistance = 3.0)
{
    new greenhouseid;
    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        greenhouseid = PlayerGreenhouseData[playerid][pGhIDs][slot];
        if(greenhouseid == INVALID_GREENHOUSE_ID)
        {
            continue;
        }
        if(IsPlayerNearOwnGreenhouse(playerid, greenhouseid, maxDistance))
        {
            GetPlayerGreenhouse(playerid, pGhLastNearbyID) = greenhouseid;
            return greenhouseid;
        }
    }
    GetPlayerGreenhouse(playerid, pGhLastNearbyID) = INVALID_GREENHOUSE_ID;
    return INVALID_GREENHOUSE_ID;
}

stock GetGrowthPercent(greenhouseid)
{
    new matureAt = GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds);
    if(matureAt <= 0)
    {
        return 0;
    }
    return (GetGreenhouse(greenhouseid, ghGrowthSeconds) * 100) / matureAt;
}

stock GetGreenhouseObjectModel(greenhouseid)
{
    if(GetGreenhouse(greenhouseid, ghState) == GH_STATE_EMPTY)
    {
        return 19377;
    }

    switch(GetGreenhouse(greenhouseid, ghStage))
    {
        case 0: return 3409;
        case 1: return 3408;
        case 2: return 3407;
    }
    return 3406;
}

stock GetGreenhouseStateName(greenhouseid, destination[], size = sizeof(destination))
{
    new currentState = _:GetGreenhouse(greenhouseid, ghState);
    switch(currentState)
    {
        case (_:GH_STATE_EMPTY): format(destination, size, "Empty");
        case (_:GH_STATE_PLANTED): format(destination, size, "Planted");
        case (_:GH_STATE_GROWING): format(destination, size, "Growing");
        case (_:GH_STATE_MATURE): format(destination, size, "Mature");
        default: format(destination, size, "Unknown");
    }
    return 1;
}

stock GetGreenhouseStatusText(greenhouseid, destination[], size = sizeof(destination))
{
    new stateName[24];
    GetGreenhouseStateName(greenhouseid, stateName, sizeof(stateName));

    if(GetGreenhouse(greenhouseid, ghState) == GH_STATE_EMPTY)
    {
        format(destination, size, "[Greenhouse #%d] %s | /ghplant | /ghupgrade | x%d",
            GetGreenhouse(greenhouseid, ghSlot) + 1,
            stateName,
            GetGreenhouse(greenhouseid, ghGrowthMultiplier)
        );
        return 1;
    }

    format(destination, size, "[Greenhouse #%d] %s | Tomatoes | %d%% | Stage %d | x%d",
        GetGreenhouse(greenhouseid, ghSlot) + 1,
        stateName,
        GetGrowthPercent(greenhouseid),
        GetGreenhouse(greenhouseid, ghStage) + 1,
        GetGreenhouse(greenhouseid, ghGrowthMultiplier)
    );
    return 1;
}

stock DestroyGreenhouseVisuals(greenhouseid)
{
    if(GetGreenhouse(greenhouseid, ghObjectID) != INVALID_STREAMER_ID)
    {
        DestroyDynamicObject(STREAMER_TAG_OBJECT:GetGreenhouse(greenhouseid, ghObjectID));
        GetGreenhouse(greenhouseid, ghObjectID) = INVALID_STREAMER_ID;
    }
    if(GetGreenhouse(greenhouseid, ghLabelID) != INVALID_STREAMER_ID)
    {
        DestroyDynamic3DTextLabel(Text3D:GetGreenhouse(greenhouseid, ghLabelID));
        GetGreenhouse(greenhouseid, ghLabelID) = INVALID_STREAMER_ID;
    }
    if(GetGreenhouse(greenhouseid, ghAreaID) != INVALID_STREAMER_ID)
    {
        DestroyDynamicArea(STREAMER_TAG_AREA:GetGreenhouse(greenhouseid, ghAreaID));
        GetGreenhouse(greenhouseid, ghAreaID) = INVALID_STREAMER_ID;
    }
    return 1;
}

stock CreateGreenhouseVisuals(greenhouseid)
{
    new text[160];
    new modelid = GetGreenhouseObjectModel(greenhouseid);
    new Float:x = GetGreenhouse(greenhouseid, ghPosX);
    new Float:y = GetGreenhouse(greenhouseid, ghPosY);
    new Float:z = GetGreenhouse(greenhouseid, ghPosZ);
    new worldid = GetGreenhouse(greenhouseid, ghWorld);
    new interiorid = GetGreenhouse(greenhouseid, ghInterior);

    GetGreenhouse(greenhouseid, ghObjectID) = _:CreateDynamicObject(modelid, x, y, z - 0.90, 0.0, 0.0, 0.0, worldid, interiorid);
    GetGreenhouseStatusText(greenhouseid, text, sizeof(text));
    GetGreenhouse(greenhouseid, ghLabelID) = _:CreateDynamic3DTextLabel(text, 0xFFFFFFFF, x, y, z + 0.50, 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, worldid, interiorid);
    GetGreenhouse(greenhouseid, ghAreaID) = _:CreateDynamicCircle(x, y, 3.0, worldid, interiorid);
    return 1;
}

stock RefreshGreenhouseVisuals(greenhouseid)
{
    if(greenhouseid == INVALID_GREENHOUSE_ID || !GetGreenhouse(greenhouseid, ghExists))
    {
        return 0;
    }
    DestroyGreenhouseVisuals(greenhouseid);
    return CreateGreenhouseVisuals(greenhouseid);
}

stock FindGreenhouseByArea(STREAMER_TAG_AREA:areaid)
{
    for(new greenhouseid = 0; greenhouseid < MAX_GREENHOUSES; greenhouseid++)
    {
        if(!GetGreenhouse(greenhouseid, ghExists))
        {
            continue;
        }
        if(GetGreenhouse(greenhouseid, ghAreaID) == areaid)
        {
            return greenhouseid;
        }
    }
    return INVALID_GREENHOUSE_ID;
}

stock CreatePlayerGreenhouse(playerid)
{
    if(!GetPlayer(playerid, pLogin))
    {
        SendClientMessage(playerid, -1, "You must be logged in first.");
        return INVALID_GREENHOUSE_ID;
    }
    if(!GetPlayerGreenhouse(playerid, pGhLoaded))
    {
        SendClientMessage(playerid, -1, "Greenhouses are not loaded yet.");
        return INVALID_GREENHOUSE_ID;
    }
    if(GetPlayerGreenhouse(playerid, pGhCount) >= MAX_PLAYER_GREENHOUSES)
    {
        SendClientMessage(playerid, -1, "You already have 5 greenhouses.");
        return INVALID_GREENHOUSE_ID;
    }

    new slot = FindFreePlayerGreenhouseSlot(playerid);
    new greenhouseid = FindFreeGreenhouseSlot();
    new Cache:result;
    new query[768];
    new mysqlError[192];
    new insertId;
    new affectedRows;
    new mysqlErrno;

    if(slot == INVALID_GREENHOUSE_ID || greenhouseid == INVALID_GREENHOUSE_ID)
    {
        SendClientMessage(playerid, -1, "No free greenhouse slots are available.");
        return INVALID_GREENHOUSE_ID;
    }

    GetGreenhouse(greenhouseid, ghExists) = true;
    GetGreenhouse(greenhouseid, ghOwnerID) = GetPlayer(playerid, pID);
    GetGreenhouse(greenhouseid, ghSlot) = slot;
    GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_NONE;
    GetGreenhouse(greenhouseid, ghUpgradeLevel) = 0;
    GetGreenhouse(greenhouseid, ghGrowthMultiplier) = 1;
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_EMPTY;
    GetGreenhouse(greenhouseid, ghStage) = 0;
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = 0;
    GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds) = 600;
    GetGreenhouse(greenhouseid, ghInterior) = GetPlayerInterior(playerid);
    GetGreenhouse(greenhouseid, ghWorld) = GetPlayerVirtualWorld(playerid);
    GetPlayerPos(playerid, GetGreenhouse(greenhouseid, ghPosX), GetGreenhouse(greenhouseid, ghPosY), GetGreenhouse(greenhouseid, ghPosZ));

    mysql_format(dbHandle, query, sizeof(query), "\
    INSERT INTO `greenhouses` (`owner_id`, `slot`, `plant_type`, `upgrade_level`, `growth_multiplier`, `state`, `stage`, `growth_seconds`, `mature_at_growth_seconds`, `pos_x`, `pos_y`, `pos_z`, `interior`, `world`) \
    VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %d, %d)",
        GetGreenhouse(greenhouseid, ghOwnerID),
        GetGreenhouse(greenhouseid, ghSlot),
        _:GetGreenhouse(greenhouseid, ghPlantType),
        GetGreenhouse(greenhouseid, ghUpgradeLevel),
        GetGreenhouse(greenhouseid, ghGrowthMultiplier),
        _:GetGreenhouse(greenhouseid, ghState),
        GetGreenhouse(greenhouseid, ghStage),
        GetGreenhouse(greenhouseid, ghGrowthSeconds),
        GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds),
        GetGreenhouse(greenhouseid, ghPosX),
        GetGreenhouse(greenhouseid, ghPosY),
        GetGreenhouse(greenhouseid, ghPosZ),
        GetGreenhouse(greenhouseid, ghInterior),
        GetGreenhouse(greenhouseid, ghWorld)
    );

    // TEMP diagnostic logging for greenhouse creation.
    printf("[GH_CREATE] creator='%s' playerid=%d owner_id=%d db='%s' handle=%d",
        GetPlayer(playerid, pName),
        playerid,
        GetGreenhouse(greenhouseid, ghOwnerID),
        MYSQL_DB,
        _:dbHandle
    );
    printf("[GH_CREATE] sql=%s", query);

    result = mysql_query(dbHandle, query, true);
    insertId = cache_insert_id();
    affectedRows = cache_affected_rows();
    mysqlErrno = mysql_errno(dbHandle);
    mysql_error(mysqlError, sizeof(mysqlError), dbHandle);
    if(result)
    {
        cache_delete(result);
    }

    printf("[GH_CREATE] result_cache=%d insert_id=%d affected_rows=%d errno=%d error='%s'",
        _:result,
        insertId,
        affectedRows,
        mysqlErrno,
        mysqlError
    );

    if(mysqlErrno != 0 || insertId <= 0 || affectedRows <= 0)
    {
        ResetGreenhouse(greenhouseid);
        SendClientMessage(playerid, -1, "Greenhouse DB insert failed. Check server logs.");
        return INVALID_GREENHOUSE_ID;
    }

    GetGreenhouse(greenhouseid, ghSQLID) = insertId;

    AttachGreenhouseToPlayer(playerid, greenhouseid);
    CreateGreenhouseVisuals(greenhouseid);
    return greenhouseid;
}

stock PlantTomatoInGreenhouse(playerid, greenhouseid)
{
    if(!IsPlayerNearOwnGreenhouse(playerid, greenhouseid))
    {
        return SendClientMessage(playerid, -1, "Move closer to your greenhouse.");
    }
    if(GetGreenhouse(greenhouseid, ghState) != GH_STATE_EMPTY)
    {
        return SendClientMessage(playerid, -1, "This greenhouse is already in use.");
    }

    GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_TOMATO;
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_PLANTED;
    GetGreenhouse(greenhouseid, ghStage) = 0;
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = 0;
    GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds) = 600;
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_STATE | GH_DIRTY_GROWTH);
    RefreshGreenhouseVisuals(greenhouseid);
    return SendClientMessage(playerid, -1, "Tomatoes planted.");
}

stock UpgradeGreenhouse(playerid, greenhouseid)
{
    if(!IsPlayerNearOwnGreenhouse(playerid, greenhouseid))
    {
        return SendClientMessage(playerid, -1, "Move closer to your greenhouse.");
    }
    if(GetGreenhouse(greenhouseid, ghGrowthMultiplier) >= 2)
    {
        return SendClientMessage(playerid, -1, "This greenhouse is already upgraded.");
    }

    GetGreenhouse(greenhouseid, ghUpgradeLevel) = 1;
    GetGreenhouse(greenhouseid, ghGrowthMultiplier) = 2;
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_UPGRADE);
    RefreshGreenhouseVisuals(greenhouseid);
    return SendClientMessage(playerid, -1, "Greenhouse upgraded to x2 growth.");
}

stock HarvestGreenhouse(playerid, greenhouseid)
{
    if(!IsPlayerNearOwnGreenhouse(playerid, greenhouseid))
    {
        return SendClientMessage(playerid, -1, "Move closer to your greenhouse.");
    }
    if(GetGreenhouse(greenhouseid, ghState) != GH_STATE_MATURE)
    {
        return SendClientMessage(playerid, -1, "The tomatoes are not ready yet.");
    }
    if(GetGreenhouse(greenhouseid, ghBusy))
    {
        return SendClientMessage(playerid, -1, "Harvest is already being processed.");
    }

    GetGreenhouse(greenhouseid, ghBusy) = true;
    GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_NONE;
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_EMPTY;
    GetGreenhouse(greenhouseid, ghStage) = 0;
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = 0;
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_STATE | GH_DIRTY_GROWTH);
    RefreshGreenhouseVisuals(greenhouseid);
    SaveGreenhouse(greenhouseid);
    GetGreenhouse(greenhouseid, ghBusy) = false;
    return SendClientMessage(playerid, -1, "Harvest collected successfully.");
}

stock ResetGreenhouseForTest(playerid, greenhouseid)
{
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Greenhouse not found.");
    }
    if(GetGreenhouse(greenhouseid, ghOwnerID) != GetPlayer(playerid, pID) && !IsPlayerAdmin(playerid))
    {
        return SendClientMessage(playerid, -1, "This is not your greenhouse.");
    }

    GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_NONE;
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_EMPTY;
    GetGreenhouse(greenhouseid, ghStage) = 0;
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = 0;
    GetGreenhouse(greenhouseid, ghBusy) = false;
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_STATE | GH_DIRTY_GROWTH);
    RefreshGreenhouseVisuals(greenhouseid);
    SaveGreenhouse(greenhouseid);
    return SendClientMessage(playerid, -1, "Greenhouse reset.");
}

stock ForceGreenhouseMature(playerid, greenhouseid)
{
    if(greenhouseid == INVALID_GREENHOUSE_ID)
    {
        return SendClientMessage(playerid, -1, "Greenhouse not found.");
    }
    if(GetGreenhouse(greenhouseid, ghOwnerID) != GetPlayer(playerid, pID) && !IsPlayerAdmin(playerid))
    {
        return SendClientMessage(playerid, -1, "This is not your greenhouse.");
    }

    if(GetGreenhouse(greenhouseid, ghState) == GH_STATE_EMPTY)
    {
        GetGreenhouse(greenhouseid, ghPlantType) = GH_PLANT_TOMATO;
    }
    GetGreenhouse(greenhouseid, ghGrowthSeconds) = GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds);
    GetGreenhouse(greenhouseid, ghState) = GH_STATE_MATURE;
    GetGreenhouse(greenhouseid, ghStage) = 3;
    MarkGreenhouseDirty(greenhouseid, GH_DIRTY_STATE | GH_DIRTY_GROWTH);
    RefreshGreenhouseVisuals(greenhouseid);
    SaveGreenhouse(greenhouseid);
    return SendClientMessage(playerid, -1, "Greenhouse forced to mature state.");
}

stock SavePlayerGreenhouses(playerid)
{
    if(!GetPlayerGreenhouse(playerid, pGhLoaded))
    {
        return 1;
    }

    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        new greenhouseid = PlayerGreenhouseData[playerid][pGhIDs][slot];
        if(greenhouseid == INVALID_GREENHOUSE_ID)
        {
            continue;
        }

        SaveGreenhouse(greenhouseid);
    }
    return 1;
}

stock UnloadPlayerGreenhouses(playerid)
{
    if(!GetPlayerGreenhouse(playerid, pGhLoaded))
    {
        return ResetPlayerGreenhouses(playerid);
    }

    for(new slot = 0; slot < MAX_PLAYER_GREENHOUSES; slot++)
    {
        new greenhouseid = PlayerGreenhouseData[playerid][pGhIDs][slot];
        if(greenhouseid == INVALID_GREENHOUSE_ID)
        {
            continue;
        }

        DestroyGreenhouseVisuals(greenhouseid);
        ResetGreenhouse(greenhouseid);
    }
    return ResetPlayerGreenhouses(playerid);
}

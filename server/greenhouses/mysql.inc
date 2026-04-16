public: LoadPlayerGreenhousesCallback(playerid)
{
    new rows;
    cache_get_row_count(rows);

    for(new row = 0; row < rows; row++)
    {
        new greenhouseid = FindFreeGreenhouseSlot();
        new plantType;
        new greenhouseState;
        if(greenhouseid == INVALID_GREENHOUSE_ID)
        {
            break;
        }

        GetGreenhouse(greenhouseid, ghExists) = true;
        cache_get_value_int(row, "id", GetGreenhouse(greenhouseid, ghSQLID));
        cache_get_value_int(row, "owner_id", GetGreenhouse(greenhouseid, ghOwnerID));
        cache_get_value_int(row, "slot", GetGreenhouse(greenhouseid, ghSlot));
        cache_get_value_int(row, "plant_type", plantType);
        cache_get_value_int(row, "upgrade_level", GetGreenhouse(greenhouseid, ghUpgradeLevel));
        cache_get_value_int(row, "growth_multiplier", GetGreenhouse(greenhouseid, ghGrowthMultiplier));
        cache_get_value_int(row, "state", greenhouseState);
        cache_get_value_int(row, "stage", GetGreenhouse(greenhouseid, ghStage));
        cache_get_value_int(row, "growth_seconds", GetGreenhouse(greenhouseid, ghGrowthSeconds));
        cache_get_value_int(row, "mature_at_growth_seconds", GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds));
        cache_get_value_float(row, "pos_x", GetGreenhouse(greenhouseid, ghPosX));
        cache_get_value_float(row, "pos_y", GetGreenhouse(greenhouseid, ghPosY));
        cache_get_value_float(row, "pos_z", GetGreenhouse(greenhouseid, ghPosZ));
        cache_get_value_int(row, "interior", GetGreenhouse(greenhouseid, ghInterior));
        cache_get_value_int(row, "world", GetGreenhouse(greenhouseid, ghWorld));

        GetGreenhouse(greenhouseid, ghPlantType) = E_GREENHOUSE_PLANT:plantType;
        GetGreenhouse(greenhouseid, ghState) = E_GREENHOUSE_STATE:greenhouseState;

        if(GetGreenhouse(greenhouseid, ghGrowthMultiplier) <= 0)
        {
            GetGreenhouse(greenhouseid, ghGrowthMultiplier) = 1;
        }
        if(GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds) <= 0)
        {
            GetGreenhouse(greenhouseid, ghMatureAtGrowthSeconds) = 600;
        }
        GetGreenhouse(greenhouseid, ghBusy) = false;
        GetGreenhouse(greenhouseid, ghDirtyFlags) = GH_DIRTY_NONE;

        AttachGreenhouseToPlayer(playerid, greenhouseid);
        CreateGreenhouseVisuals(greenhouseid);
    }

    GetPlayerGreenhouse(playerid, pGhLoaded) = true;
    return CompletePlayerAuthorization(playerid);
}

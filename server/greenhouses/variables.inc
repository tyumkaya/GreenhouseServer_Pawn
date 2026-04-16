#define MAX_PLAYER_GREENHOUSES (5)
#define MAX_GREENHOUSES (MAX_PLAYERS * MAX_PLAYER_GREENHOUSES)

#define INVALID_GREENHOUSE_ID (-1)

#define GH_DIRTY_NONE (0)
#define GH_DIRTY_STATE (1)
#define GH_DIRTY_GROWTH (1 << 1)
#define GH_DIRTY_UPGRADE (1 << 2)
#define GH_DIRTY_POSITION (1 << 3)
#define GH_DIRTY_ALL (GH_DIRTY_STATE | GH_DIRTY_GROWTH | GH_DIRTY_UPGRADE | GH_DIRTY_POSITION)

enum E_GREENHOUSE_STATE
{
    GH_STATE_EMPTY,
    GH_STATE_PLANTED,
    GH_STATE_GROWING,
    GH_STATE_MATURE
}

enum E_GREENHOUSE_PLANT
{
    GH_PLANT_NONE,
    GH_PLANT_TOMATO
}

enum GreenhouseInformation
{
    bool:ghExists,
    ghSQLID,
    ghOwnerID,
    ghSlot,
    E_GREENHOUSE_PLANT:ghPlantType,
    ghUpgradeLevel,
    ghGrowthMultiplier,
    E_GREENHOUSE_STATE:ghState,
    ghStage,
    ghGrowthSeconds,
    ghMatureAtGrowthSeconds,
    Float:ghPosX,
    Float:ghPosY,
    Float:ghPosZ,
    ghInterior,
    ghWorld,
    ghObjectID,
    ghLabelID,
    ghAreaID,
    bool:ghBusy,
    ghDirtyFlags
}
new GreenhouseData[MAX_GREENHOUSES][GreenhouseInformation];

enum PlayerGreenhouseInformation
{
    bool:pGhLoaded,
    pGhCount,
    pGhLastNearbyID,
    pGhIDs[MAX_PLAYER_GREENHOUSES]
}
new PlayerGreenhouseData[MAX_PLAYERS][PlayerGreenhouseInformation];

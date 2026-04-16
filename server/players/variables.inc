enum PlayerInformation{
    pID,
    pName[MAX_PLAYER_NAME],
    pPassword[20],
    bool:pLogin,
}
new InfoPlayer[MAX_PLAYERS][PlayerInformation];
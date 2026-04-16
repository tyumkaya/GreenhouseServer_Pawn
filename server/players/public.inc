public: TimerConnectToServer(playerid)
{
    static const fmt_query[] = "SELECT `id` FROM `users` WHERE `name` = '%s'";
    new query[sizeof(fmt_query) + MAX_PLAYER_NAME];
    mysql_format(dbHandle, query, sizeof(query), fmt_query, GetPlayer(playerid, pName));
    
    return mysql_tquery(dbHandle, query, "CheckPlayerToBase", "i", playerid);
    
}
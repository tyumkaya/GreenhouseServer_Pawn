public OnGameModeInit()
{
    dbHandle = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB);
    if(mysql_errno(dbHandle) != 0) printf("#%d Failed to connect Database");
    else print("Database connected successfully");

    greenhouseUpdateTick = 0;
    greenhouseUpdateTimer = SetTimer("ProcessGreenhouses", 1000, true);

	SendRconCommand("hostname "SERVER_HOST"");
	SetGameModeText(SERVER_MODE);
	AddPlayerClass(0, 2495.3547, -1688.2319, 13.6774, 351.1646, WEAPON_M4, 500, WEAPON_KNIFE, 1, WEAPON_COLT45, 100);
	AddStaticVehicle(522, 2493.7583, -1683.6482, 12.9099, 270.8069, -1, -1);
	return 1;
}

public OnGameModeExit()
{
	if(greenhouseUpdateTimer)
	{
		KillTimer(greenhouseUpdateTimer);
	}
	SaveAllGreenhouses();
	mysql_close(dbHandle);
	return 1;
}

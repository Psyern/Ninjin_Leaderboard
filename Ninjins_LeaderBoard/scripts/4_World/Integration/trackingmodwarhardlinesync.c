// Syncs DME-WAR and Expansion Hardline data into PlayerDeathData
// so the 3_Game web export can include it without cross-module references.
// Called on player connect, disconnect, and periodically via the export timer.

class TrackingModWarHardlineSync
{
	static void SyncPlayerData(string plainId, PlayerDeathData playerData)
	{
		if (!playerData)
			return;

		SyncWarData(plainId, playerData);
		SyncHardlineData(plainId, playerData);
	}

	protected static void SyncWarData(string plainId, PlayerDeathData playerData)
	{
		#ifdef DME_War
		DMEW_PlayerState warState = DMEW_Storage.LoadOrCreate(plainId);
		if (warState)
		{
			playerData.WarFaction = warState.Faction;
			playerData.WarAlignment = warState.Alignment;
			playerData.WarLevel = warState.Level;
			playerData.WarBossKills = 0;
		}
		#endif
	}

	protected static void SyncHardlineData(string plainId, PlayerDeathData playerData)
	{
		#ifdef EXPANSIONMODHARDLINE
		PlayerBase player = PlayerBase.GetPlayerByUID(plainId);
		if (player)
		{
			playerData.HardlineReputation = player.Expansion_GetReputation();
		}
		#endif
	}

	static void SyncAllPlayers()
	{
		if (!g_TrackingModData || !g_TrackingModData.m_PlayerDataMap)
			return;

		int count = g_TrackingModData.m_PlayerDataMap.Count();
		for (int i = 0; i < count; i++)
		{
			string pid = g_TrackingModData.m_PlayerDataMap.GetKey(i);
			PlayerDeathData pd = g_TrackingModData.m_PlayerDataMap.GetElement(i);
			if (pd)
				SyncPlayerData(pid, pd);
		}
	}

	static void SyncGlobalFactionPoints(TrackingModWebLeaderboardExport exportData)
	{
		if (!exportData)
			return;

		#ifdef DME_War
		DMEW_FactionPointsData fpData = DMEW_FactionPoints.Get();
		if (fpData)
		{
			exportData.globalEastPoints = fpData.EastPoints;
			exportData.globalWestPoints = fpData.WestPoints;
		}
		#endif
	}
}

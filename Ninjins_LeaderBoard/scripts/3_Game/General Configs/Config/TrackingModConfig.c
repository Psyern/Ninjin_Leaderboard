ref TrackingModConfig g_TrackingModConfig;
const string TRACKING_MOD_CONFIG_FILE = TRACKING_MOD_DATA_DIR + "LeaderBoardConfig.json";
class TrackingModConfig
{
	autoptr array<string> AdminIDs;
	int DeletePlayerFilesOlderThanDays;
	bool UseUTCForDates;
	bool EnableRewardSystem;
	bool DisablePVPLeaderboard;
	bool DisablePVELeaderboard;
	int MaxPVEPlayersDisplay;
	int MaxPVPPlayersDisplay;
	bool ShowPlayerOnlineStatusPVE;
	bool ShowPlayerOnlineStatusPVP;
	string SurvivorIconPathMale;
	string SurvivorIconPathFemale;
	bool DisableDeathBySuicide;
	bool DisableDeathByGrenade;
	bool DisableDeathByTrap;
	bool DisableDeathByZombie;
	bool DisableDeathByAnimal;
	bool DisableDeathByAI;
	bool DisableDeathByCar;
	bool DisableDeathByWeapon;
	bool DisableDeathByUnarmed;
	bool DisableDeathByUnknown;
	bool DisableKillByGrenade;
	bool DisableKillByTrap;
	bool DisableKillByAnimal;
	bool DisableKillByZombie;
	bool DisableKillByCar;
	bool DisableKillByWeapon;
	bool DisableKillByUnarmed;
	bool DisableKillByUnknown;
	bool DisableKillByUnconsciousSuicide;
	autoptr array<string> TrackPVEKillsInZoneTypes;
	bool ExcludePVPKillWhenBothInPVE;
	bool EnableZoneKillExclusions;
	autoptr array<ref ZoneKillExclusion> ExcludeKillsInZones;
	bool EnableWebExport;
	string WebExportFileName;
	bool WebExportIncludePlayerIDs;
	int WebExportMaxPlayers;
	void TrackingModConfig()
	{
		AdminIDs = new array<string>();
		AdminIDs.Insert("76561198043039918");
		DeletePlayerFilesOlderThanDays = 0;
		UseUTCForDates = false;
		EnableRewardSystem = false;
		DisablePVPLeaderboard = false;
		DisablePVELeaderboard = false;
		MaxPVEPlayersDisplay = 10;
		MaxPVPPlayersDisplay = 24;
		ShowPlayerOnlineStatusPVE = true;
		ShowPlayerOnlineStatusPVP = true;
		SurvivorIconPathMale = "Ninjins_LeaderBoard/gui/icons/survivor/male.edds";
		SurvivorIconPathFemale = "Ninjins_LeaderBoard/gui/icons/survivor/female.edds";
		DisableDeathBySuicide = false;
		DisableDeathByGrenade = false;
		DisableDeathByTrap = false;
		DisableDeathByZombie = false;
		DisableDeathByAnimal = false;
		DisableDeathByAI = false;
		DisableDeathByCar = false;
		DisableDeathByWeapon = false;
		DisableDeathByUnarmed = false;
		DisableDeathByUnknown = false;
		DisableKillByGrenade = false;
		DisableKillByTrap = false;
		DisableKillByAnimal = false;
		DisableKillByZombie = false;
		DisableKillByCar = false;
		DisableKillByWeapon = false;
		DisableKillByUnarmed = false;
		DisableKillByUnknown = false;
		DisableKillByUnconsciousSuicide = false;
		TrackPVEKillsInZoneTypes = new array<string>();
		TrackPVEKillsInZoneTypes.Insert("everywhere");
		ExcludePVPKillWhenBothInPVE = false;
		EnableZoneKillExclusions = false;
		ExcludeKillsInZones = new array<ref ZoneKillExclusion>();
		EnableWebExport = true;
		WebExportFileName = "LeaderboardWebExport.json";
		WebExportIncludePlayerIDs = false;
		WebExportMaxPlayers = 100;
		ZoneKillExclusion example1;
		ZoneKillExclusion example2;
		ZoneKillExclusion example3;
		
		example1 = new ZoneKillExclusion();
		example1.ZoneType = "pve";
		example1.ExcludedClassNames = new array<string>();
		example1.ExcludedClassNames.Insert("PlayerBase");
		example1.ZoneNames = new array<string>();
		ExcludeKillsInZones.Insert(example1);
		example2 = new ZoneKillExclusion();
		example2.ZoneType = "pvp";
		example2.ExcludedClassNames = new array<string>();
		example2.ExcludedClassNames.Insert("PlayerBase");
		example2.ZoneNames = new array<string>();
		example2.ZoneNames.Insert("FlagPvPZone_");
		example2.ZoneNames.Insert("KOTHZone_");
		ExcludeKillsInZones.Insert(example2);
		example3 = new ZoneKillExclusion();
		example3.ZoneType = "safezone";
		example3.ExcludedClassNames = new array<string>();
		example3.ExcludedClassNames.Insert("PlayerBase");
		example3.ExcludedClassNames.Insert("AnimalBase");
		example3.ExcludedClassNames.Insert("ZombieBase");
		example3.ExcludedClassNames.Insert("eAIBase");
		example3.ZoneNames = new array<string>();
		ExcludeKillsInZones.Insert(example3);
	}
	void SaveConfig()
	{
		if (!GetGame().IsDedicatedServer())
		{
			return;
		}
		TrackingModData.CheckDirectories();
		JsonFileLoader<TrackingModConfig>.JsonSaveFile(TRACKING_MOD_CONFIG_FILE, this);
	}
	static TrackingModConfig LoadConfig()
	{
		if (!GetGame().IsDedicatedServer())
		{
			return null;
		}
		TrackingModData.CheckDirectories();
		TrackingModConfig config;
		if (FileExist(TRACKING_MOD_CONFIG_FILE))
		{
			config = new TrackingModConfig();
			JsonFileLoader<TrackingModConfig>.JsonLoadFile(TRACKING_MOD_CONFIG_FILE, config);
			if (config.MaxPVEPlayersDisplay > 100)
				config.MaxPVEPlayersDisplay = 100;
			if (config.MaxPVPPlayersDisplay > 100)
				config.MaxPVPPlayersDisplay = 100;
			if (config.WebExportFileName == "")
				config.WebExportFileName = "LeaderboardWebExport.json";
			if (config.WebExportMaxPlayers < 1)
				config.WebExportMaxPlayers = 1;
			if (config.WebExportMaxPlayers > 1000)
				config.WebExportMaxPlayers = 1000;
			Print("[TrackingMod] Config loaded from: " + TRACKING_MOD_CONFIG_FILE);
			Print("[TrackingMod] Loaded values - MaxPVEPlayersDisplay: " + config.MaxPVEPlayersDisplay.ToString() + ", MaxPVPPlayersDisplay: " + config.MaxPVPPlayersDisplay.ToString() + ", ShowPlayerOnlineStatusPVE: " + config.ShowPlayerOnlineStatusPVE.ToString() + ", ShowPlayerOnlineStatusPVP: " + config.ShowPlayerOnlineStatusPVP.ToString());
		}
		else
		{
			config = new TrackingModConfig();
			Print("[TrackingMod] LeaderBoardConfig.json not found. Creating default config.");
			config.SaveConfig();
		}
		return config;
	}
	bool IsAdmin(string playerID)
	{
		if (!AdminIDs)
			return false;
		for (int i = 0; i < AdminIDs.Count(); i++)
		{
			if (AdminIDs[i] == playerID)
				return true;
		}
		return false;
	}
	bool ShouldExcludeDeathCause(int deathCause)
	{
		bool result;
		result = false;
		switch (deathCause)
		{
			case 0: result = DisableDeathBySuicide; break;
			case 1: result = DisableDeathByGrenade; break;
			case 2: result = DisableDeathByTrap; break;
			case 3: result = DisableDeathByZombie; break;
			case 4: result = DisableDeathByAnimal; break;
			case 5: result = DisableDeathByAI; break;
			case 6: result = DisableDeathByCar; break;
			case 7: result = DisableDeathByWeapon; break;
			case 8: result = DisableDeathByUnarmed; break;
			case 9: result = DisableDeathByUnknown; break;
			default: result = false; break;
		}
		return result;
	}
	bool ShouldExcludeKillCause(int killCause)
	{
		bool result;
		result = false;
		switch (killCause)
		{
			case 0: result = DisableKillByGrenade; break;
			case 1: result = DisableKillByTrap; break;
			case 2: result = DisableKillByAnimal; break;
			case 3: result = DisableKillByZombie; break;
			case 4: result = DisableKillByCar; break;
			case 5: result = DisableKillByWeapon; break;
			case 6: result = DisableKillByUnarmed; break;
			case 7: result = DisableKillByUnknown; break;
			case 8: result = DisableKillByUnconsciousSuicide; break;
			default: result = false; break;
		}
		return result;
	}
	bool ShouldTrackPVEKill(string playerID)
	{
		if (!TrackPVEKillsInZoneTypes || TrackPVEKillsInZoneTypes.Count() == 0)
			return true;
		if (TrackPVEKillsInZoneTypes.Find("everywhere") != -1)
			return true;
		#ifdef NinjinsPvPPvE
		if (TrackPVEKillsInZoneTypes.Find("pvp") != -1 && TrackingModZoneUtils.IsPlayerInPVPZone(playerID))
			return true;
		if (TrackPVEKillsInZoneTypes.Find("pve") != -1 && TrackingModZoneUtils.IsPlayerInPVEZone(playerID))
			return true;
		if (TrackPVEKillsInZoneTypes.Find("visual") != -1 && TrackingModZoneUtils.IsPlayerInVisualZone(playerID))
			return true;
		if (TrackPVEKillsInZoneTypes.Find("raid") != -1 && TrackingModZoneUtils.IsPlayerInRaidZone(playerID))
			return true;
		#endif
		return false;
	}
	bool ShouldExcludeKillInZone(string zoneType, string className, Object entity, string playerID)
	{
		#ifdef NinjinsPvPPvE
		string currentZoneName;
		bool playerInZone;
		#endif
		int j;
		int i;
		ZoneKillExclusion exclusion;
		
		if (!EnableZoneKillExclusions)
			return false;
		if (!ExcludeKillsInZones || ExcludeKillsInZones.Count() == 0)
			return false;
		#ifdef NinjinsPvPPvE
		currentZoneName = TrackingModZoneUtils.GetPlayerZoneName(playerID);
		#endif
		for (i = 0; i < ExcludeKillsInZones.Count(); i++)
		{
			exclusion = ExcludeKillsInZones[i];
			if (!exclusion || exclusion.ZoneType != zoneType)
				continue;
			#ifdef NinjinsPvPPvE
			playerInZone = false;
			if (zoneType == "pvp")
				playerInZone = TrackingModZoneUtils.IsPlayerInPVPZone(playerID);
			else if (zoneType == "pve")
				playerInZone = TrackingModZoneUtils.IsPlayerInPVEZone(playerID);
			else if (zoneType == "visual")
				playerInZone = TrackingModZoneUtils.IsPlayerInVisualZone(playerID);
			else if (zoneType == "raid")
				playerInZone = TrackingModZoneUtils.IsPlayerInRaidZone(playerID);
			else if (zoneType == "safezone")
				playerInZone = TrackingModZoneUtils.IsPlayerInPVEZone(playerID);
			if (!playerInZone)
				continue;
			#endif
			if (!exclusion.ExcludedClassNames || exclusion.ExcludedClassNames.Count() == 0)
				continue;
			bool classMatches = false;
			for (j = 0; j < exclusion.ExcludedClassNames.Count(); j++)
			{
				string excludedClass = exclusion.ExcludedClassNames[j];
				if (className == excludedClass)
				{
					classMatches = true;
					break;
				}
				#ifdef NinjinsPvPPvE
				if (entity && entity.IsKindOf(excludedClass))
				{
					classMatches = true;
					break;
				}
				#endif
			}
			if (classMatches)
			{
				#ifdef NinjinsPvPPvE
				if (!exclusion.ZoneNames || exclusion.ZoneNames.Count() == 0)
				{
					return true;
				}
				for (j = 0; j < exclusion.ZoneNames.Count(); j++)
				{
					if (TrackingModZoneUtils.MatchesZoneName(currentZoneName, exclusion.ZoneNames[j]))
						return true;
				}
				#else
				return true;
				#endif
			}
		}
		return false;
	}
}
class ZoneKillExclusion
{
	string ZoneType;
	autoptr array<string> ExcludedClassNames;
	autoptr array<string> ZoneNames;
	void ZoneKillExclusion()
	{
		ZoneType = "";
		ExcludedClassNames = new array<string>();
		ZoneNames = new array<string>();
	}
}
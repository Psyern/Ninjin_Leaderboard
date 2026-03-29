class TrackingModGeneralAdminData
{
	ref array<string> AdminIDs;
	int DeletePlayerFilesOlderThanDays;
	bool UseUTCForDates;
	bool EnableRewardSystem;
	bool DisablePVPLeaderboard;
	bool DisablePVELeaderboard;
	int MaxPVEPlayersDisplay;
	int MaxPVPPlayersDisplay;
	bool ShowPlayerOnlineStatusPVE;
	bool ShowPlayerOnlineStatusPVP;
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
	ref array<string> TrackPVEKillsInZoneTypes;
	bool ExcludePVPKillWhenBothInPVE;
	bool EnableWebExport;
	int WebExportIntervalSeconds;
	int WebExportMaxPlayers;
	bool WebExportIncludePlayerIDs;

	void TrackingModGeneralAdminData()
	{
		AdminIDs = new array<string>();
		TrackPVEKillsInZoneTypes = new array<string>();
		DeletePlayerFilesOlderThanDays = 0;
		UseUTCForDates = false;
		EnableRewardSystem = false;
		DisablePVPLeaderboard = false;
		DisablePVELeaderboard = false;
		MaxPVEPlayersDisplay = 10;
		MaxPVPPlayersDisplay = 24;
		ShowPlayerOnlineStatusPVE = true;
		ShowPlayerOnlineStatusPVP = true;
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
		ExcludePVPKillWhenBothInPVE = false;
		EnableWebExport = false;
		WebExportIntervalSeconds = 300;
		WebExportMaxPlayers = 100;
		WebExportIncludePlayerIDs = true;
	}
}

class TrackingModPVEAdminData
{
	int PVEDeathPenaltyPoints;
	ref array<ref PVECategory> Categories;

	void TrackingModPVEAdminData()
	{
		PVEDeathPenaltyPoints = 5;
		Categories = new array<ref PVECategory>();
	}
}

class TrackingModPVPAdminData
{
	int PVPDeathPenaltyPoints;
	ref array<ref PVPCategory> Categories;

	void TrackingModPVPAdminData()
	{
		PVPDeathPenaltyPoints = 10;
		Categories = new array<ref PVPCategory>();
	}
}

class TrackingModAdminConfigData
{
	ref TrackingModGeneralAdminData GeneralSettings;
	ref TrackingModPVEAdminData PVESettings;
	ref TrackingModPVPAdminData PVPSettings;

	void TrackingModAdminConfigData()
	{
		GeneralSettings = new TrackingModGeneralAdminData();
		PVESettings = new TrackingModPVEAdminData();
		PVPSettings = new TrackingModPVPAdminData();
	}
}

class TrackingModAdminSaveResponse
{
	bool Success;
	string Message;

	void TrackingModAdminSaveResponse()
	{
		Success = false;
		Message = "";
	}
}

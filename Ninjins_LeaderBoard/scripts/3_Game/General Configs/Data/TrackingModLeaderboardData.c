#ifndef TRACKING_MOD_LEADERBOARD_DATA
#define TRACKING_MOD_LEADERBOARD_DATA

class TrackingModLeaderboardPlayerData
{
	string playerID;
	string playerName;
	int deathCount;
	int pvePoints;
	int pvpPoints;
	int isOnline;
	string survivorType;
	int pendingRewards;
	ref map<string, int> categoryKills;
	ref map<string, int> categoryDeaths;
	ref map<string, int> categoryLongestRanges;
	int shotsFired;
	int shotsHit;
	int headshots;
	float distanceTravelled;
	float distanceOnFoot;
	float distanceInVehicle;
	int totalDeaths;
	int suicides;
	float accuracy;
	float headshotPercentage;

	void TrackingModLeaderboardPlayerData()
	{
		playerID = "";
		playerName = "";
		deathCount = 0;
		pvePoints = 0;
		pvpPoints = 0;
		isOnline = 0;
		survivorType = "";
		pendingRewards = 0;
		categoryKills = new map<string, int>();
		categoryDeaths = new map<string, int>();
		categoryLongestRanges = new map<string, int>();
		shotsFired = 0;
		shotsHit = 0;
		headshots = 0;
		distanceTravelled = 0.0;
		distanceOnFoot = 0.0;
		distanceInVehicle = 0.0;
		totalDeaths = 0;
		suicides = 0;
		accuracy = 0.0;
		headshotPercentage = 0.0;
	}
}

class TrackingModLeaderboardData
{
	int playerOnlineCounter;
	bool showPlayerOnlineStatusPVE;
	bool showPlayerOnlineStatusPVP;
	bool disablePVPLeaderboard;
	bool disablePVELeaderboard;
	ref array<ref TrackingModLeaderboardPlayerData> topPVEPlayers;
	ref array<ref TrackingModLeaderboardPlayerData> topPVPPlayers;
	ref map<string, string> categoryPreviews;
	string survivorIconPathMale;
	string survivorIconPathFemale;
	int currentPage;
	int totalPages;
	int totalPlayers;
	int maxDisplayCount;
	int playersOnCurrentPage;
	bool showShotsFired;
	bool showShotsHit;
	bool showHeadshots;
	bool showHeadshotPercentage;
	bool showDistanceTravelled;
	bool showAccuracy;

	void TrackingModLeaderboardData()
	{
		playerOnlineCounter = 0;
		showPlayerOnlineStatusPVE = true;
		showPlayerOnlineStatusPVP = true;
		disablePVPLeaderboard = false;
		disablePVELeaderboard = false;
		topPVEPlayers = new array<ref TrackingModLeaderboardPlayerData>();
		topPVPPlayers = new array<ref TrackingModLeaderboardPlayerData>();
		categoryPreviews = new map<string, string>();
		survivorIconPathMale = "";
		survivorIconPathFemale = "";
		currentPage = 1;
		totalPages = 1;
		totalPlayers = 0;
		maxDisplayCount = 0;
		playersOnCurrentPage = 0;
		showShotsFired = true;
		showShotsHit = true;
		showHeadshots = true;
		showHeadshotPercentage = true;
		showDistanceTravelled = true;
		showAccuracy = true;
	}
}

class TrackingModRewardClaimResponse
{
	bool success;
	int remainingRewards;
	string message;
	
	void TrackingModRewardClaimResponse()
	{
		success = false;
		remainingRewards = 0;
		message = "";
	}
}

class TrackingModWebLeaderboardPlayerData
{
	string playerID;
	string playerName;
	int deathCount;
	int pveDeaths;
	int pvpDeaths;
	int pvePoints;
	int pvpPoints;
	int isOnline;
	string survivorType;
	string lastLoginDate;
	int pendingRewards;
	ref map<string, int> categoryKills;
	ref map<string, int> categoryDeaths;
	ref map<string, int> categoryLongestRanges;

	// DME-WAR fields
	string warFaction;
	int warAlignment;
	int warLevel;
	int warBossKills;

	// Expansion Hardline field
	int hardlineReputation;

	// Extended stats
	int shotsFired;
	int shotsHit;
	int headshots;
	float distanceTravelled;

	void TrackingModWebLeaderboardPlayerData()
	{
		playerID = "";
		playerName = "";
		deathCount = 0;
		pveDeaths = 0;
		pvpDeaths = 0;
		pvePoints = 0;
		pvpPoints = 0;
		isOnline = 0;
		survivorType = "";
		lastLoginDate = "";
		pendingRewards = 0;
		categoryKills = new map<string, int>();
		categoryDeaths = new map<string, int>();
		categoryLongestRanges = new map<string, int>();
		warFaction = "";
		warAlignment = 0;
		warLevel = 0;
		warBossKills = 0;
		hardlineReputation = 0;
		shotsFired = 0;
		shotsHit = 0;
		headshots = 0;
		distanceTravelled = 0.0;
	}
}

class TrackingModWebLeaderboardExport
{
	string generatedAt;
	int playerOnlineCounter;
	int totalPlayers;
	bool disablePVPLeaderboard;
	bool disablePVELeaderboard;
	bool includePlayerIDs;
	int exportPlayerLimit;
	int globalEastPoints;
	int globalWestPoints;
	ref array<ref TrackingModWebLeaderboardPlayerData> topPVEPlayers;
	ref array<ref TrackingModWebLeaderboardPlayerData> topPVPPlayers;
	ref map<string, string> categoryPreviews;

	void TrackingModWebLeaderboardExport()
	{
		generatedAt = "";
		playerOnlineCounter = 0;
		totalPlayers = 0;
		disablePVPLeaderboard = false;
		disablePVELeaderboard = false;
		includePlayerIDs = false;
		exportPlayerLimit = 0;
		globalEastPoints = 0;
		globalWestPoints = 0;
		topPVEPlayers = new array<ref TrackingModWebLeaderboardPlayerData>();
		topPVPPlayers = new array<ref TrackingModWebLeaderboardPlayerData>();
		categoryPreviews = new map<string, string>();
	}
}

#endif

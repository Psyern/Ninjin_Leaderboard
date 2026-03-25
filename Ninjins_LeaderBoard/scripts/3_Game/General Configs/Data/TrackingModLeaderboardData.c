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

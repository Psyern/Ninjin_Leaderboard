const string TRACKING_MOD_ROOT_FOLDER = "$profile:Ninjins_Tracking_Mod\\";
const string TRACKING_MOD_DATA_DIR = TRACKING_MOD_ROOT_FOLDER + "Data\\";
const string TRACKING_MOD_PLAYERS_DIR = TRACKING_MOD_DATA_DIR + "Players\\";
const string TRACKING_MOD_WEB_EXPORT_FILE = TRACKING_MOD_DATA_DIR + "LeaderboardWebExport.json";

static string GetPlayerFilePath(string playerID)
{
	return TRACKING_MOD_PLAYERS_DIR + playerID + ".json";
}

class PlayerDeathData
{
	string PlayerID;
	string PlayerName;
	vector LastDeathPosition;
	string LastKillerName;
	string LastKillerType;
	
	ref map<string, int> CategoryKills;
	ref map<string, int> CategoryLongestRanges;
	ref map<string, int> CategoryDeaths;
	int PvEPoints;
	int PvPPoints;
	int playerIsOnline;
	string survivorType;
	string LastLoginDate;
	ref map<string, ref array<int>> AvailableMilestones;
	ref map<string, ref array<int>> ClaimedMilestones;

	// DME-WAR integration (populated from 4_World via #ifdef DME_War)
	string WarFaction;
	int WarAlignment;
	int WarLevel;
	int WarBossKills;

	// Expansion Hardline integration (populated from 4_World via #ifdef EXPANSIONMODHARDLINE)
	int HardlineReputation;

	// Extended stats tracking
	int ShotsFired;
	int ShotsHit;
	int Headshots;
	float DistanceTravelled;

	void PlayerDeathData()
	{
		PlayerID = "";
		PlayerName = "";
		LastDeathPosition = "0 0 0";
		LastKillerName = "Unknown";
		LastKillerType = "Unknown";
		CategoryKills = new map<string, int>();
		CategoryLongestRanges = new map<string, int>();
		CategoryDeaths = new map<string, int>();
		PvEPoints = 0;
		PvPPoints = 0;
		playerIsOnline = 0;
		survivorType = "";
		LastLoginDate = "";
		AvailableMilestones = new map<string, ref array<int>>();
		ClaimedMilestones = new map<string, ref array<int>>();
		WarFaction = "";
		WarAlignment = 0;
		WarLevel = 0;
		WarBossKills = 0;
		HardlineReputation = 0;
		ShotsFired = 0;
		ShotsHit = 0;
		Headshots = 0;
		DistanceTravelled = 0.0;
	}
	
	int GetPVPPoints()
	{
		return PvPPoints;
	}
	
	int GetPVEPoints()
	{
		return PvEPoints;
	}
	
	void UpdatePVPPoints()
	{
		PVPCategoryConfig categoryConfig;
		array<ref PVPCategory> categories;
		int i;
		PVPCategory category;
		int categoryKills;
		int categoryDeaths;
		string categoryID;
		int basePoints;
		int multiplier;
		int j;
		string classNameWithSettings;
		int firstColonIndex;
		string remainingStr;
		int secondColonIndex;
		string basePointsStr;
		string multiplierStr;
		int kill;
		ref map<string, bool> processedCategories;
		
		PvPPoints = 0;
		categoryConfig = PVPCategoryConfig.GetInstance();
		if (!categoryConfig)
			return;
		categories = categoryConfig.GetCategories();
		processedCategories = new map<string, bool>();
		
		if (!CategoryKills)
		{
			for (i = 0; i < categories.Count(); i++)
			{
				category = categories[i];
				if (category && category.CategoryID != "")
				{
					categoryID = category.CategoryID;
					if (processedCategories.Contains(categoryID))
						continue;
					processedCategories.Set(categoryID, true);
					categoryDeaths = GetCategoryDeaths(categoryID);
					PvPPoints -= categoryDeaths * categoryConfig.GetPVPDeathPenaltyPoints();
				}
			}
			
			if (PvPPoints < 0)
				PvPPoints = 0;
			return;
		}
		
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories[i];
			if (category && category.CategoryID != "")
			{
				categoryID = category.CategoryID;
				if (processedCategories.Contains(categoryID))
					continue;
				processedCategories.Set(categoryID, true);
				categoryKills = GetCategoryKills(categoryID);
				if (categoryKills > 0)
				{
					basePoints = 100;
					multiplier = 1;
					
					if (category.ClassNames && category.ClassNames.Count() > 0)
					{
						for (j = 0; j < category.ClassNames.Count(); j++)
						{
							classNameWithSettings = category.ClassNames[j];
							
							firstColonIndex = classNameWithSettings.IndexOf(":");
							if (firstColonIndex != -1)
							{
								remainingStr = classNameWithSettings.Substring(firstColonIndex + 1, classNameWithSettings.Length() - (firstColonIndex + 1));
								
								secondColonIndex = remainingStr.IndexOf(":");
								if (secondColonIndex == -1)
								{
									basePoints = remainingStr.ToInt();
									multiplier = 1;
								}
								else
								{
									basePointsStr = remainingStr.Substring(0, secondColonIndex);
									multiplierStr = remainingStr.Substring(secondColonIndex + 1, remainingStr.Length() - (secondColonIndex + 1));
									basePoints = basePointsStr.ToInt();
									multiplier = multiplierStr.ToInt();
								}
								break;
							}
						}
					}
					
					for (kill = 1; kill <= categoryKills; kill++)
					{
						PvPPoints += basePoints * multiplier;
					}
				}
				
				categoryDeaths = GetCategoryDeaths(categoryID);
				PvPPoints -= categoryDeaths * categoryConfig.GetPVPDeathPenaltyPoints();
			}
		}
		
		if (PvPPoints < 0)
			PvPPoints = 0;
	}
	
	void UpdatePVEPoints()
	{
		PVECategoryConfig categoryConfig;
		array<ref PVECategory> categories;
		int i;
		PVECategory category;
		int categoryKills;
		int categoryDeaths;
		string categoryID;
		int basePoints;
		int multiplier;
		int j;
		string classNameWithSettings;
		int firstColonIndex;
		string remainingStr;
		int secondColonIndex;
		string basePointsStr;
		string multiplierStr;
		int kill;
		int deathPenaltyPoints;
		ref map<string, bool> processedCategories;
		
		PvEPoints = 0;
		deathPenaltyPoints = 1; // Default if config not loaded
		
		categoryConfig = PVECategoryConfig.GetInstance();
		if (categoryConfig)
		{
			deathPenaltyPoints = categoryConfig.GetPVEDeathPenaltyPoints();
		}
		
		if (!CategoryKills)
		{
			// If no kills, still apply death penalties
			if (categoryConfig)
			{
				categories = categoryConfig.GetCategories();
				processedCategories = new map<string, bool>();
				for (i = 0; i < categories.Count(); i++)
				{
					category = categories[i];
					if (category && category.CategoryID != "")
					{
						categoryID = category.CategoryID;
						
						// Skip if we've already processed this category ID (handles duplicates)
						if (processedCategories.Contains(categoryID))
							continue;
						
						processedCategories.Set(categoryID, true);
						categoryDeaths = GetCategoryDeaths(categoryID);
						PvEPoints -= categoryDeaths * deathPenaltyPoints;
					}
				}
			}
			
			if (PvEPoints < 0)
				PvEPoints = 0;
			return;
		}
		
		categories = categoryConfig.GetCategories();
		processedCategories = new map<string, bool>();
		
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories[i];
			if (category && category.ClassNames)
			{
				categoryID = category.CategoryID;
				
				// Skip if we've already processed this category ID (handles duplicates)
				if (processedCategories.Contains(categoryID))
					continue;
				
				processedCategories.Set(categoryID, true);
				categoryKills = GetCategoryKills(categoryID);
				
				if (categoryKills > 0)
				{
					basePoints = 0;
					multiplier = 1;
					
					for (j = 0; j < category.ClassNames.Count(); j++)
					{
						classNameWithSettings = category.ClassNames[j];
						
						firstColonIndex = classNameWithSettings.IndexOf(":");
						if (firstColonIndex != -1)
						{
							remainingStr = classNameWithSettings.Substring(firstColonIndex + 1, classNameWithSettings.Length() - (firstColonIndex + 1));
							
							secondColonIndex = remainingStr.IndexOf(":");
							if (secondColonIndex == -1)
							{
								basePoints = remainingStr.ToInt();
								multiplier = 1;
							}
							else
							{
								basePointsStr = remainingStr.Substring(0, secondColonIndex);
								multiplierStr = remainingStr.Substring(secondColonIndex + 1, remainingStr.Length() - (secondColonIndex + 1));
								basePoints = basePointsStr.ToInt();
								multiplier = multiplierStr.ToInt();
							}
							break;
						}
					}
					
					for (kill = 1; kill <= categoryKills; kill++)
					{
						PvEPoints += basePoints * multiplier;
					}
				}
				
				categoryDeaths = GetCategoryDeaths(categoryID);
				PvEPoints -= categoryDeaths * deathPenaltyPoints;
			}
		}
		
		if (PvEPoints < 0)
			PvEPoints = 0;
	}
	
	int GetCategoryKills(string categoryID)
	{
		if (!CategoryKills)
			CategoryKills = new map<string, int>();
		
		if (CategoryKills.Contains(categoryID))
			return CategoryKills.Get(categoryID);
		
		return GetLegacyPlayerCategoryValue(CategoryKills, categoryID);
	}

	int GetCategoryLongestRange(string categoryID)
	{
		int longestRange;
		
		if (!CategoryLongestRanges)
			CategoryLongestRanges = new map<string, int>();
		
		if (CategoryLongestRanges.Contains(categoryID))
			return CategoryLongestRanges.Get(categoryID);
		
		longestRange = GetLegacyPlayerCategoryLongestRange(categoryID);
		return longestRange;
	}

	protected bool IsPrimaryPVPPlayerCategory(string categoryID)
	{
		PVPCategoryConfig pvpCategoryConfig;
		
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return categoryID == "Players";
		
		return categoryID == pvpCategoryConfig.GetPrimaryPlayerCategoryID();
	}

	protected int GetLegacyPlayerCategoryValue(map<string, int> categoryMap, string categoryID)
	{
		int totalValue;
		
		if (!categoryMap || !IsPrimaryPVPPlayerCategory(categoryID))
			return 0;
		
		totalValue = 0;
		if (categoryMap.Contains("Players"))
			totalValue += categoryMap.Get("Players");
		if (categoryMap.Contains("Player"))
			totalValue += categoryMap.Get("Player");
		if (categoryMap.Contains("Spieler"))
			totalValue += categoryMap.Get("Spieler");
		if (categoryMap.Contains("Survivor"))
			totalValue += categoryMap.Get("Survivor");
		
		return totalValue;
	}

	protected int GetLegacyPlayerCategoryLongestRange(string categoryID)
	{
		int longestRange;
		
		if (!CategoryLongestRanges || !IsPrimaryPVPPlayerCategory(categoryID))
			return 0;
		
		longestRange = 0;
		if (CategoryLongestRanges.Contains("Players") && CategoryLongestRanges.Get("Players") > longestRange)
			longestRange = CategoryLongestRanges.Get("Players");
		if (CategoryLongestRanges.Contains("Player") && CategoryLongestRanges.Get("Player") > longestRange)
			longestRange = CategoryLongestRanges.Get("Player");
		if (CategoryLongestRanges.Contains("Spieler") && CategoryLongestRanges.Get("Spieler") > longestRange)
			longestRange = CategoryLongestRanges.Get("Spieler");
		if (CategoryLongestRanges.Contains("Survivor") && CategoryLongestRanges.Get("Survivor") > longestRange)
			longestRange = CategoryLongestRanges.Get("Survivor");
		
		return longestRange;
		
	}
	
	int GetCategoryDeaths(string categoryID)
	{
		if (!CategoryDeaths)
			CategoryDeaths = new map<string, int>();
		
		if (CategoryDeaths.Contains(categoryID))
			return CategoryDeaths.Get(categoryID);
		
		return GetLegacyPlayerCategoryValue(CategoryDeaths, categoryID);
		
	}
	
	int GetTotalPVPDeaths()
	{
		PVPCategoryConfig pvpCategoryConfig;
		array<ref PVPCategory> categories;
		int i;
		PVPCategory category;
		string categoryID;
		int totalDeaths;
		ref map<string, bool> processedCategories;
		
		totalDeaths = 0;
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return 0;
		
		categories = pvpCategoryConfig.GetCategories();
		if (!categories)
			return 0;
		processedCategories = new map<string, bool>();
		
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories[i];
			if (category && category.CategoryID != "")
			{
				categoryID = category.CategoryID;
				if (processedCategories.Contains(categoryID))
					continue;
				processedCategories.Set(categoryID, true);
				totalDeaths += GetCategoryDeaths(categoryID);
			}
		}
		
		return totalDeaths;
	}
	
	int GetTotalPVEDeaths()
	{
		PVECategoryConfig pveCategoryConfig;
		array<ref PVECategory> categories;
		int i;
		PVECategory category;
		string categoryID;
		int totalDeaths;
		
		totalDeaths = 0;
		pveCategoryConfig = PVECategoryConfig.GetInstance();
		if (!pveCategoryConfig)
			return 0;
		
		categories = pveCategoryConfig.GetCategories();
		if (!categories)
			return 0;
		
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories[i];
			if (category && category.CategoryID != "")
			{
				categoryID = category.CategoryID;
				totalDeaths += GetCategoryDeaths(categoryID);
			}
		}
		
		return totalDeaths;
	}
	
	void AddCategoryDeath(string categoryID)
	{
		int currentDeaths;
		
		if (!CategoryDeaths)
			CategoryDeaths = new map<string, int>();
		
		currentDeaths = GetCategoryDeaths(categoryID);
		CategoryDeaths.Set(categoryID, currentDeaths + 1);
		Print("[TrackingMod] CategoryDeaths[" + categoryID + "] = " + (currentDeaths + 1).ToString());
	}
	
	void AddCategoryKill(string categoryID, float range, int multiplier = 1)
	{
		int currentKills;
		int currentLongest;
		PVPCategoryConfig pvpCategoryConfig;
		
		if (!CategoryKills)
			CategoryKills = new map<string, int>();
		
		currentKills = GetCategoryKills(categoryID);
		CategoryKills.Set(categoryID, currentKills + 1);
		Print("[TrackingMod] CategoryKills[" + categoryID + "] = " + (currentKills + 1).ToString());
		
		if (!CategoryLongestRanges)
			CategoryLongestRanges = new map<string, int>();
		
		currentLongest = 0;
		if (CategoryLongestRanges.Contains(categoryID))
			currentLongest = CategoryLongestRanges.Get(categoryID);
		
		if (range && currentLongest < range)
		{
			CategoryLongestRanges.Set(categoryID, Math.Round(range));
			Print("[TrackingMod] CategoryLongestRanges[" + categoryID + "] = " + Math.Round(range).ToString());
		}
		
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (pvpCategoryConfig.HasCategory(categoryID))
		{
			UpdatePVPPoints();
		}
		else
		{
			UpdatePVEPoints();
		}
		
		// Check if any milestones were reached and add to AvailableMilestones
		CheckAndAddMilestones(categoryID, currentKills + 1);
	}
	
	void AddShotFired()
	{
		ShotsFired = ShotsFired + 1;
	}

	void AddShotHit(bool isHeadshot)
	{
		ShotsHit = ShotsHit + 1;
		if (isHeadshot)
		{
			Headshots = Headshots + 1;
		}
	}

	void AddDistanceTravelled(float distance)
	{
		DistanceTravelled = DistanceTravelled + distance;
	}

	float GetAccuracy()
	{
		float accuracy;

		if (ShotsFired <= 0)
			return 0.0;
		accuracy = (ShotsHit * 100.0) / ShotsFired;
		accuracy = Math.Round(accuracy * 100.0) / 100.0;
		return accuracy;
	}

	float GetHeadshotPercentage()
	{
		float pct;

		if (ShotsHit <= 0)
			return 0.0;
		pct = (Headshots * 100.0) / ShotsHit;
		pct = Math.Round(pct * 100.0) / 100.0;
		return pct;
	}

	void CheckAndAddMilestones(string categoryID, int newKillCount)
	{
		array<int> milestones;
		array<int> availableMilestones;
		array<int> claimedMilestones;
		int milestoneIdx;
		int milestone;
		int availableIdx;
		bool alreadyAvailable;
		bool alreadyClaimed;
		int claimedIdx;
		string broadcastMsg;

		if (!g_TrackingModRewardConfig || !g_TrackingModRewardConfig.EnableMilestoneRewards)
			return;
		
		if (!AvailableMilestones)
			AvailableMilestones = new map<string, ref array<int>>();
		
		TrackingMod.LogDebug("[CheckAndAddMilestones] Checking milestones for category: " + categoryID + " with kill count: " + newKillCount.ToString());
		
		// Check PvE Milestones
		if (g_TrackingModRewardConfig.PVEMilestones && g_TrackingModRewardConfig.PVEMilestones.Contains(categoryID))
		{
			milestones = g_TrackingModRewardConfig.PVEMilestones.Get(categoryID);
			if (milestones)
			{
				for (milestoneIdx = 0; milestoneIdx < milestones.Count(); milestoneIdx++)
				{
					milestone = milestones.Get(milestoneIdx);
					if (newKillCount >= milestone)
					{
						// Check if already claimed
						alreadyClaimed = false;
						if (ClaimedMilestones && ClaimedMilestones.Contains(categoryID))
						{
							claimedMilestones = ClaimedMilestones.Get(categoryID);
							if (claimedMilestones)
							{
								for (claimedIdx = 0; claimedIdx < claimedMilestones.Count(); claimedIdx++)
								{
									if (claimedMilestones.Get(claimedIdx) == milestone)
									{
										alreadyClaimed = true;
										break;
									}
								}
							}
						}
						
						// Check if already available
						alreadyAvailable = false;
						if (AvailableMilestones.Contains(categoryID))
						{
							availableMilestones = AvailableMilestones.Get(categoryID);
							if (availableMilestones)
							{
								for (availableIdx = 0; availableIdx < availableMilestones.Count(); availableIdx++)
								{
									if (availableMilestones.Get(availableIdx) == milestone)
									{
										alreadyAvailable = true;
										break;
									}
								}
							}
						}
						
						// Add to AvailableMilestones if not already claimed or available
						if (!alreadyClaimed && !alreadyAvailable)
						{
							if (!AvailableMilestones.Contains(categoryID))
							{
								availableMilestones = new array<int>();
								AvailableMilestones.Set(categoryID, availableMilestones);
							}
							else
							{
								availableMilestones = AvailableMilestones.Get(categoryID);
							}
							
							if (availableMilestones)
							{
								availableMilestones.Insert(milestone);
								TrackingMod.LogInfo("[CheckAndAddMilestones] Added milestone " + milestone.ToString() + " for category " + categoryID + " to AvailableMilestones");
								if (g_TrackingModRewardConfig && g_TrackingModRewardConfig.EnableServerwideMilestoneNotification)
								{
									broadcastMsg = string.Format(g_TrackingModRewardConfig.ServerwideMilestoneMessage, PlayerName, milestone.ToString(), categoryID);
									NotificationSystem.Create(new StringLocaliser("Kill Streak!"), new StringLocaliser(broadcastMsg), "set:dayz_gui image:notify_special", ARGB(255, 255, 215, 0), 8.0, null);
								}
							}
						}
					}
				}
			}
		}
		
		// Check PvP Milestones
		if (g_TrackingModRewardConfig.PVPMilestones && g_TrackingModRewardConfig.PVPMilestones.Contains(categoryID))
		{
			milestones = g_TrackingModRewardConfig.PVPMilestones.Get(categoryID);
			if (milestones)
			{
				for (milestoneIdx = 0; milestoneIdx < milestones.Count(); milestoneIdx++)
				{
					milestone = milestones.Get(milestoneIdx);
					if (newKillCount >= milestone)
					{
						// Check if already claimed
						alreadyClaimed = false;
						if (ClaimedMilestones && ClaimedMilestones.Contains(categoryID))
						{
							claimedMilestones = ClaimedMilestones.Get(categoryID);
							if (claimedMilestones)
							{
								for (claimedIdx = 0; claimedIdx < claimedMilestones.Count(); claimedIdx++)
								{
									if (claimedMilestones.Get(claimedIdx) == milestone)
									{
										alreadyClaimed = true;
										break;
									}
								}
							}
						}
						
						// Check if already available
						alreadyAvailable = false;
						if (AvailableMilestones.Contains(categoryID))
						{
							availableMilestones = AvailableMilestones.Get(categoryID);
							if (availableMilestones)
							{
								for (availableIdx = 0; availableIdx < availableMilestones.Count(); availableIdx++)
								{
									if (availableMilestones.Get(availableIdx) == milestone)
									{
										alreadyAvailable = true;
										break;
									}
								}
							}
						}
						
						// Add to AvailableMilestones if not already claimed or available
						if (!alreadyClaimed && !alreadyAvailable)
						{
							if (!AvailableMilestones.Contains(categoryID))
							{
								availableMilestones = new array<int>();
								AvailableMilestones.Set(categoryID, availableMilestones);
							}
							else
							{
								availableMilestones = AvailableMilestones.Get(categoryID);
							}
							
							if (availableMilestones)
							{
								availableMilestones.Insert(milestone);
								TrackingMod.LogInfo("[CheckAndAddMilestones] Added milestone " + milestone.ToString() + " for category " + categoryID + " to AvailableMilestones");
								if (g_TrackingModRewardConfig && g_TrackingModRewardConfig.EnableServerwideMilestoneNotification)
								{
									broadcastMsg = string.Format(g_TrackingModRewardConfig.ServerwideMilestoneMessage, PlayerName, milestone.ToString(), categoryID);
									NotificationSystem.Create(new StringLocaliser("Kill Streak!"), new StringLocaliser(broadcastMsg), "set:dayz_gui image:notify_special", ARGB(255, 255, 215, 0), 8.0, null);
								}
							}
						}
					}
				}
			}
		}
	}
	
	void CheckAllMilestones()
	{
		string categoryID;
		int kills;
		int categoryIdx;
		
		if (!CategoryKills || CategoryKills.Count() == 0)
			return;
		
		// Check all categories for milestones
		for (categoryIdx = 0; categoryIdx < CategoryKills.Count(); categoryIdx++)
		{
			categoryID = CategoryKills.GetKey(categoryIdx);
			kills = CategoryKills.GetElement(categoryIdx);
			if (kills > 0)
			{
				CheckAndAddMilestones(categoryID, kills);
			}
		}
	}
}

ref TrackingModData g_TrackingModData;

class TrackingModData
{
	autoptr map<string, ref PlayerDeathData> m_PlayerDataMap;
	autoptr array<ref PlayerDeathData> m_CachedSortedPVE;
	autoptr array<ref PlayerDeathData> m_CachedSortedPVP;
	bool m_LeaderboardDataDirty;
	int m_LastSortTime;
	
	void TrackingModData()
	{
		m_PlayerDataMap = new map<string, ref PlayerDeathData>();
		m_CachedSortedPVE = new array<ref PlayerDeathData>();
		m_CachedSortedPVP = new array<ref PlayerDeathData>();
		m_LeaderboardDataDirty = true;
		m_LastSortTime = 0;
	}
	
	void MarkLeaderboardDirty()
	{
		m_LeaderboardDataDirty = true;
	}
	
	array<ref PlayerDeathData> GetSortedPVEList()
	{
		if (m_LeaderboardDataDirty || m_CachedSortedPVE.Count() == 0)
		{
			RebuildSortedLists();
		}
		return m_CachedSortedPVE;
	}
	
	array<ref PlayerDeathData> GetSortedPVPList()
	{
		if (m_LeaderboardDataDirty || m_CachedSortedPVP.Count() == 0)
		{
			RebuildSortedLists();
		}
		return m_CachedSortedPVP;
	}
	
	void RebuildSortedLists()
	{
		int idx;
		PlayerDeathData playerDataEntry;
		
		if (!m_PlayerDataMap)
			return;
		
		m_CachedSortedPVE.Clear();
		m_CachedSortedPVP.Clear();
		
		for (idx = 0; idx < m_PlayerDataMap.Count(); idx++)
		{
			playerDataEntry = m_PlayerDataMap.GetElement(idx);
			if (playerDataEntry)
			{
				playerDataEntry.UpdatePVEPoints();
				playerDataEntry.UpdatePVPPoints();
				m_CachedSortedPVE.Insert(playerDataEntry);
				m_CachedSortedPVP.Insert(playerDataEntry);
			}
		}
		
		SortPlayersByPVEPoints(m_CachedSortedPVE);
		SortPlayersByPVPPoints(m_CachedSortedPVP);
		
		m_LeaderboardDataDirty = false;
		m_LastSortTime = GetGame().GetTime();
	}
	
	void SortPlayersByPVEPoints(array<ref PlayerDeathData> players)
	{
		int i;
		int j;
		PlayerDeathData player1;
		PlayerDeathData player2;
		int points1;
		int points2;
		
		for (i = 0; i < players.Count() - 1; i++)
		{
			for (j = 0; j < players.Count() - i - 1; j++)
			{
				player1 = players.Get(j);
				player2 = players.Get(j + 1);
				
				if (player1 && player2)
				{
					points1 = player1.GetPVEPoints();
					points2 = player2.GetPVEPoints();
					
					if (points1 < points2)
					{
						players.SwapItems(j, j + 1);
					}
				}
			}
		}
	}
	
	void SortPlayersByPVPPoints(array<ref PlayerDeathData> players)
	{
		int i;
		int j;
		PlayerDeathData player1;
		PlayerDeathData player2;
		int points1;
		int points2;
		
		for (i = 0; i < players.Count() - 1; i++)
		{
			for (j = 0; j < players.Count() - i - 1; j++)
			{
				player1 = players.Get(j);
				player2 = players.Get(j + 1);
				
				if (player1 && player2)
				{
					points1 = player1.GetPVPPoints();
					points2 = player2.GetPVPPoints();
					
					if (points1 < points2)
					{
						players.SwapItems(j, j + 1);
					}
				}
			}
		}
	}
	
	static void CheckDirectories()
	{
		if (!FileExist(TRACKING_MOD_ROOT_FOLDER))
		{
			if (MakeDirectory(TRACKING_MOD_ROOT_FOLDER))
				TrackingMod.LogDebug("Created root directory: " + TRACKING_MOD_ROOT_FOLDER);
		}
		
		if (!FileExist(TRACKING_MOD_DATA_DIR))
		{
			if (MakeDirectory(TRACKING_MOD_DATA_DIR))
				TrackingMod.LogDebug("Created data directory: " + TRACKING_MOD_DATA_DIR);
		}
		
		if (!FileExist(TRACKING_MOD_PLAYERS_DIR))
		{
			if (MakeDirectory(TRACKING_MOD_PLAYERS_DIR))
				TrackingMod.LogDebug("Created players directory: " + TRACKING_MOD_PLAYERS_DIR);
		}
		
		TrackingMod.InitLogFile();
	}
	
	static TrackingModData LoadData()
	{
		TrackingModData data;
		
		if (g_TrackingModData)
			return g_TrackingModData;
		
		data = new TrackingModData();
		g_TrackingModData = data;
		
		if (GetGame().IsDedicatedServer())
		{
			data.LoadAllPlayerFiles();
		}
		else
		{
			data.m_PlayerDataMap = new map<string, ref PlayerDeathData>();
		}
		
		return data;
	}
	
	static TrackingModData GetInstance()
	{
		if (!g_TrackingModData)
		{
			g_TrackingModData = new TrackingModData();
			if (!GetGame().IsDedicatedServer())
			{
				g_TrackingModData.m_PlayerDataMap = new map<string, ref PlayerDeathData>();
			}
		}
		return g_TrackingModData;
	}
	
	void SyncPlayerData(string playerID, PlayerDeathData playerData)
	{
		if (!m_PlayerDataMap)
			m_PlayerDataMap = new map<string, ref PlayerDeathData>();
		
		if (playerData && playerID != "")
		{
			m_PlayerDataMap.Set(playerID, playerData);
			MarkLeaderboardDirty();
		}
	}
	
	
	static void ReloadData()
	{
		if (!g_TrackingModData)
			return;
		
		if (GetGame().IsDedicatedServer())
		{
			g_TrackingModData.m_PlayerDataMap.Clear();
			g_TrackingModData.LoadAllPlayerFiles();
			Print("[TrackingMod] Player data reloaded from files");
		}
	}
	
	void LoadAllPlayerFiles()
	{
		int loadedCount;
		array<ref CF_File> playerFiles;
		string jsonPattern;
		CF_File playerFile;
		string filePath;
		string fileName;
		string playerID;
		PlayerDeathData playerData;
		int offlineCount;
		int i;
		PlayerDeathData loadedPlayerData;
		CF_Date currentDate;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		if (!m_PlayerDataMap)
		{
			m_PlayerDataMap = new map<string, ref PlayerDeathData>();
		}
		
		loadedCount = 0;
		playerFiles = new array<ref CF_File>();
		jsonPattern = TRACKING_MOD_PLAYERS_DIR + "*.json";
		
		if (CF_Directory.GetFiles(jsonPattern, playerFiles, 2))
		{
			Print("[TrackingMod] Found " + playerFiles.Count().ToString() + " player files, loading into memory...");
			
			for (i = 0; i < playerFiles.Count(); i++)
			{
				playerFile = playerFiles.Get(i);
				filePath = playerFile.GetFullPath();
				fileName = playerFile.GetFileName();
				
				if (fileName && fileName.Length() > 5)
				{
					playerID = fileName;
					playerID = playerID.Substring(0, playerID.Length() - 5);
					
					if (playerID != "" && FileExist(filePath))
					{
						playerData = new PlayerDeathData();
						JsonFileLoader<PlayerDeathData>.JsonLoadFile(filePath, playerData);
						if (playerData && playerData.PlayerID != "")
						{
							if (playerData.playerIsOnline != 0 && playerData.playerIsOnline != 1)
							{
								playerData.playerIsOnline = 0;
							}
						if (playerData.LastLoginDate == "")
						{
							bool useUTC;
							if (g_TrackingModConfig)
								useUTC = g_TrackingModConfig.UseUTCForDates;
							else
								useUTC = false;
							currentDate = CF_Date.Now(useUTC);
							playerData.LastLoginDate = currentDate.ToString(CF_Date.DATETIME);
							JsonFileLoader<PlayerDeathData>.JsonSaveFile(filePath, playerData);
						}
						playerData.UpdatePVEPoints();
						playerData.UpdatePVPPoints();
						if (playerData.PvPPoints < 0)
							playerData.PvPPoints = 0;
						
						// Initialize AvailableMilestones if not present (for old player files)
						if (!playerData.AvailableMilestones)
							playerData.AvailableMilestones = new map<string, ref array<int>>();
						
						// Initialize ClaimedMilestones if not present (for old player files)
						if (!playerData.ClaimedMilestones)
							playerData.ClaimedMilestones = new map<string, ref array<int>>();
						
						// Check and populate AvailableMilestones for existing kills
						if (playerData.CategoryKills && playerData.CategoryKills.Count() > 0)
						{
							playerData.CheckAllMilestones();
						}
						
						m_PlayerDataMap.Set(playerID, playerData);
						loadedCount++;
						}
					}
				}
			}
		}
		
		Print("[TrackingMod] Loaded " + loadedCount.ToString() + " player files into memory (total players: " + m_PlayerDataMap.Count().ToString() + ")");
		
		offlineCount = 0;
		for (i = 0; i < m_PlayerDataMap.Count(); i++)
		{
			loadedPlayerData = m_PlayerDataMap.GetElement(i);
			if (loadedPlayerData && loadedPlayerData.playerIsOnline == 1)
			{
				loadedPlayerData.playerIsOnline = 0;
				offlineCount++;
			}
		}
		
		if (offlineCount > 0)
		{
			Print("[TrackingMod] Set " + offlineCount.ToString() + " players to offline (server restart/crash)");
			SaveData();
		}
	}
	
	void SaveData()
	{
		int i;
		string playerID;
		PlayerDeathData playerData;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		if (!m_PlayerDataMap)
			return;
		
		for (i = 0; i < m_PlayerDataMap.Count(); i++)
		{
			playerID = m_PlayerDataMap.GetKey(i);
			playerData = m_PlayerDataMap.GetElement(i);
			if (playerData && playerID != "")
			{
				SavePlayerData(playerData, playerID, true);
			}
		}
		
		ExportWebLeaderboard();
	}
	
	static void ForceSave()
	{
		if (g_TrackingModData)
		{
			g_TrackingModData.SaveData();
		}
	}
	
	
	PlayerDeathData GetPlayerData(string playerID, string playerName = "")
	{
		PlayerDeathData playerData;
		string filePath;
		
		if (!m_PlayerDataMap)
		{
			m_PlayerDataMap = new map<string, ref PlayerDeathData>();
		}
		
		if (m_PlayerDataMap.Contains(playerID))
		{
			playerData = m_PlayerDataMap.Get(playerID);
			if (playerName != "" && playerData.PlayerName != playerName)
			{
				playerData.PlayerName = playerName;
			}
			return playerData;
		}
		
		playerData = new PlayerDeathData();
		filePath = GetPlayerFilePath(playerID);
		
		if (FileExist(filePath))
		{
			JsonFileLoader<PlayerDeathData>.JsonLoadFile(filePath, playerData);
		}
		
		if (playerData.PlayerID == "")
		{
			playerData.PlayerID = playerID;
		}
		
		if (playerName != "" && playerData.PlayerName != playerName)
		{
			playerData.PlayerName = playerName;
		}
		
		if (playerData.PvPPoints < 0)
			playerData.PvPPoints = 0;
		
		// Initialize AvailableMilestones if not present (for old player files)
		if (!playerData.AvailableMilestones)
			playerData.AvailableMilestones = new map<string, ref array<int>>();
		
		// Initialize ClaimedMilestones if not present (for old player files)
		if (!playerData.ClaimedMilestones)
			playerData.ClaimedMilestones = new map<string, ref array<int>>();
		
		// Check and populate AvailableMilestones for existing kills
		if (playerData.CategoryKills && playerData.CategoryKills.Count() > 0)
		{
			playerData.CheckAllMilestones();
		}
		
		m_PlayerDataMap.Set(playerID, playerData);
		return playerData;
	}
	
	PlayerDeathData GetOrCreatePlayer(string plainID, string playerName = "", out bool newPlayer = false)
	{
		PlayerDeathData playerData;
		string filePath;
		
		newPlayer = false;
		playerData = GetPlayerData(plainID, playerName);
		
		filePath = GetPlayerFilePath(plainID);
		if (!FileExist(filePath))
		{
			newPlayer = true;
			playerData.PlayerID = plainID;
			if (playerName != "")
			{
				playerData.PlayerName = playerName;
			}
			SavePlayerData(playerData, plainID);
		}
		
		return playerData;
	}
	
	void SavePlayerData(PlayerDeathData playerData, string plainID, bool skipWebExport = false)
	{
		string filePath;
		bool wasNewPlayer;
		bool dataChanged;
		
		if (!playerData || playerData.PlayerID == "" || plainID == "")
			return;
		
		wasNewPlayer = !m_PlayerDataMap.Contains(plainID);
		playerData.PlayerID = plainID;
		filePath = GetPlayerFilePath(plainID);
		
		if (GetGame().IsDedicatedServer())
		{
			JsonFileLoader<PlayerDeathData>.JsonSaveFile(filePath, playerData);
		}
		
		if (m_PlayerDataMap)
		{
			m_PlayerDataMap.Set(plainID, playerData);
			dataChanged = wasNewPlayer || (playerData.CategoryKills && playerData.CategoryKills.Count() > 0) || (playerData.CategoryDeaths && playerData.CategoryDeaths.Count() > 0);
			if (dataChanged)
			{
				MarkLeaderboardDirty();
			}
		}
		
		if (!skipWebExport)
		{
			ExportWebLeaderboard();
		}
	}
	
	void ExportWebLeaderboard()
	{
		TrackingModWebLeaderboardExport exportData;
		array<ref PlayerDeathData> sortedPVE;
		array<ref PlayerDeathData> sortedPVP;
		int playerLimit;
		bool includePlayerIDs;
		bool useUTC;
		CF_Date currentDate;
		PVECategoryConfig categoryConfig;
		array<ref PVECategory> categories;
		int idx;
		PVECategory category;
		TrackingModWebLeaderboardPlayerData exportPlayer;
		string exportFilePath;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		if (!g_TrackingModConfig || !g_TrackingModConfig.EnableWebExport)
			return;
		
		if (!m_PlayerDataMap)
			return;
		
		playerLimit = g_TrackingModConfig.WebExportMaxPlayers;
		if (playerLimit < 1)
			playerLimit = 1;
		includePlayerIDs = g_TrackingModConfig.WebExportIncludePlayerIDs;
		useUTC = g_TrackingModConfig.UseUTCForDates;
		currentDate = CF_Date.Now(useUTC);
		exportFilePath = TRACKING_MOD_WEB_EXPORT_FILE;
		if (g_TrackingModConfig.WebExportFileName != "")
			exportFilePath = TRACKING_MOD_DATA_DIR + g_TrackingModConfig.WebExportFileName;
		
		sortedPVE = GetSortedPVEList();
		sortedPVP = GetSortedPVPList();
		exportData = new TrackingModWebLeaderboardExport();
		exportData.generatedAt = currentDate.ToString(CF_Date.DATETIME);
		exportData.playerOnlineCounter = GetOnlinePlayerCount();
		exportData.totalPlayers = m_PlayerDataMap.Count();
		exportData.disablePVELeaderboard = g_TrackingModConfig.DisablePVELeaderboard;
		exportData.disablePVPLeaderboard = g_TrackingModConfig.DisablePVPLeaderboard;
		exportData.includePlayerIDs = includePlayerIDs;
		exportData.exportPlayerLimit = playerLimit;
		exportData.globalEastPoints = 0;
		exportData.globalWestPoints = 0;
		
		categoryConfig = PVECategoryConfig.GetInstance();
		if (categoryConfig)
		{
			categories = categoryConfig.GetCategories();
			for (idx = 0; idx < categories.Count(); idx++)
			{
				category = categories.Get(idx);
				if (category && category.CategoryID != "" && category.ClassNamePreview != "")
				{
					exportData.categoryPreviews.Set(category.CategoryID, category.ClassNamePreview);
				}
			}
		}
		
		for (idx = 0; idx < sortedPVE.Count() && idx < playerLimit; idx++)
		{
			exportPlayer = CreateWebLeaderboardPlayerData(sortedPVE.Get(idx), includePlayerIDs);
			if (exportPlayer)
				exportData.topPVEPlayers.Insert(exportPlayer);
		}
		
		for (idx = 0; idx < sortedPVP.Count() && idx < playerLimit; idx++)
		{
			exportPlayer = CreateWebLeaderboardPlayerData(sortedPVP.Get(idx), includePlayerIDs);
			if (exportPlayer)
				exportData.topPVPPlayers.Insert(exportPlayer);
		}
		
		JsonFileLoader<TrackingModWebLeaderboardExport>.JsonSaveFile(exportFilePath, exportData);
	}
	
	protected int GetOnlinePlayerCount()
	{
		int idx;
		int onlineCount;
		PlayerDeathData playerData;
		
		onlineCount = 0;
		for (idx = 0; idx < m_PlayerDataMap.Count(); idx++)
		{
			playerData = m_PlayerDataMap.GetElement(idx);
			if (playerData && playerData.playerIsOnline == 1)
				onlineCount++;
		}
		
		return onlineCount;
	}
	
	protected TrackingModWebLeaderboardPlayerData CreateWebLeaderboardPlayerData(PlayerDeathData playerData, bool includePlayerIDs)
	{
		TrackingModWebLeaderboardPlayerData exportPlayer;
		int idx;
		string key;
		int value;
		
		if (!playerData)
			return null;
		
		exportPlayer = new TrackingModWebLeaderboardPlayerData();
		if (includePlayerIDs)
			exportPlayer.playerID = playerData.PlayerID;
		exportPlayer.playerName = playerData.PlayerName;
		exportPlayer.deathCount = playerData.GetTotalPVPDeaths();
		exportPlayer.pveDeaths = playerData.GetTotalPVEDeaths();
		exportPlayer.pvpDeaths = playerData.GetTotalPVPDeaths();
		exportPlayer.pvePoints = playerData.GetPVEPoints();
		exportPlayer.pvpPoints = playerData.GetPVPPoints();
		exportPlayer.isOnline = playerData.playerIsOnline;
		exportPlayer.survivorType = playerData.survivorType;
		exportPlayer.lastLoginDate = playerData.LastLoginDate;
		if (g_TrackingModRewardConfig && g_TrackingModRewardConfig.EnableMilestoneRewards)
			exportPlayer.pendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
		
		if (playerData.CategoryKills)
		{
			for (idx = 0; idx < playerData.CategoryKills.Count(); idx++)
			{
				key = playerData.CategoryKills.GetKey(idx);
				value = playerData.CategoryKills.GetElement(idx);
				exportPlayer.categoryKills.Set(key, value);
			}
		}
		
		if (playerData.CategoryDeaths)
		{
			for (idx = 0; idx < playerData.CategoryDeaths.Count(); idx++)
			{
				key = playerData.CategoryDeaths.GetKey(idx);
				value = playerData.CategoryDeaths.GetElement(idx);
				exportPlayer.categoryDeaths.Set(key, value);
			}
		}
		
		if (playerData.CategoryLongestRanges)
		{
			for (idx = 0; idx < playerData.CategoryLongestRanges.Count(); idx++)
			{
				key = playerData.CategoryLongestRanges.GetKey(idx);
				value = playerData.CategoryLongestRanges.GetElement(idx);
				exportPlayer.categoryLongestRanges.Set(key, value);
			}
		}

		exportPlayer.warFaction = playerData.WarFaction;
		exportPlayer.warAlignment = playerData.WarAlignment;
		exportPlayer.warLevel = playerData.WarLevel;
		exportPlayer.warBossKills = playerData.WarBossKills;
		exportPlayer.hardlineReputation = playerData.HardlineReputation;
		exportPlayer.shotsFired = playerData.ShotsFired;
		exportPlayer.shotsHit = playerData.ShotsHit;
		exportPlayer.headshots = playerData.Headshots;
		exportPlayer.distanceTravelled = playerData.DistanceTravelled;

		return exportPlayer;
	}
	
	void UpdateLastLoginDate(PlayerDeathData playerData)
	{
		CF_Date currentDate;
		bool useUTC;
		
		if (!playerData)
			return;
		
		if (g_TrackingModConfig)
			useUTC = g_TrackingModConfig.UseUTCForDates;
		else
			useUTC = false;
		currentDate = CF_Date.Now(useUTC);
		playerData.LastLoginDate = currentDate.ToString(CF_Date.DATETIME);
	}
	
	void UpdatePlayerDeathInfo(string playerID, string playerName, vector deathPosition, string killerName, string killerType, bool isSelfInflicted, bool killedByPlayer, bool killedByZombie, bool killedByAnimal, bool killedByAI, int environmentDeathType = -1)
	{
		PlayerDeathData playerData;
		PVPCategoryConfig pvpCategoryConfig;
		array<string> matchingCategories;
		int pvpIdx;
		string categoryID;
		PVECategoryConfig pveCategoryConfig;
		array<ref CategoryMatch> matches;
		int pveIdx;
		CategoryMatch match;
		
		playerData = GetPlayerData(playerID, playerName);
		playerData.PlayerName = playerName;
		playerData.LastDeathPosition = deathPosition;
		playerData.LastKillerName = killerName;
		playerData.LastKillerType = killerType;
		
		if (isSelfInflicted)
		{
			playerData.AddCategoryDeath("SelfInflicted");
			
			if (environmentDeathType == 1)
			{
				playerData.AddCategoryDeath("Explosion");
			}
			else if (environmentDeathType == 2)
			{
				playerData.AddCategoryDeath("OtherEnvironment");
			}
			SavePlayerData(playerData, playerID);
			return;
		}
		
		if (killedByPlayer)
		{
			pvpCategoryConfig = PVPCategoryConfig.GetInstance();
			
			if (killerType != "")
			{
				matchingCategories = pvpCategoryConfig.GetCategoryIDsForClass(killerType, null);
				
				if (matchingCategories.Count() > 0)
				{
					for (pvpIdx = 0; pvpIdx < matchingCategories.Count(); pvpIdx++)
					{
						categoryID = matchingCategories[pvpIdx];
						playerData.AddCategoryDeath(categoryID);
						Print("[TrackingMod] PVP death added to category: " + categoryID + " (victim died by player, killer class: " + killerType + ")");
					}
				}
				else
				{
					categoryID = pvpCategoryConfig.GetPrimaryPlayerCategoryID();
					playerData.AddCategoryDeath(categoryID);
					Print("[TrackingMod] PVP death added to default PVP category: " + categoryID + " (victim died by player, killer class: " + killerType + " - no PVP category match found)");
				}
			}
			else
			{
				categoryID = pvpCategoryConfig.GetPrimaryPlayerCategoryID();
				playerData.AddCategoryDeath(categoryID);
				Print("[TrackingMod] PVP death added to default PVP category: " + categoryID + " (victim died by player)");
			}
		}
		else if (killedByZombie || killedByAnimal || killedByAI)
		{
			if (killerType != "")
			{
				pveCategoryConfig = PVECategoryConfig.GetInstance();
				matches = pveCategoryConfig.GetCategoryMatchesForClass(killerType, null);
				
				if (matches.Count() > 0)
				{
					for (pveIdx = 0; pveIdx < matches.Count(); pveIdx++)
					{
						match = matches[pveIdx];
						if (match)
						{
							playerData.AddCategoryDeath(match.CategoryID);
							Print("[TrackingMod] Death added to category: " + match.CategoryID + " (killer class: " + killerType + ")");
						}
					}
				}
				else
				{
					if (killedByZombie)
						playerData.AddCategoryDeath("Zombies");
					else if (killedByAnimal)
						playerData.AddCategoryDeath("Animals");
					else if (killedByAI)
						playerData.AddCategoryDeath("AI");
				}
			}
			else
			{
				if (killedByZombie)
					playerData.AddCategoryDeath("Zombies");
				else if (killedByAnimal)
					playerData.AddCategoryDeath("Animals");
				else if (killedByAI)
					playerData.AddCategoryDeath("AI");
			}
		}
		else
		{
			string envCategoryID;
			bool isPVECategory;
			
			if (environmentDeathType == 0)
			{
				envCategoryID = "HeatDamage";
			}
			else if (environmentDeathType == 1)
			{
				envCategoryID = "Explosion";
			}
			else if (environmentDeathType == 2)
			{
				envCategoryID = "OtherEnvironment";
			}
			
			if (envCategoryID != "")
			{
				playerData.AddCategoryDeath(envCategoryID);
				
				// Check if this environmental category exists in PVE categories
				if (pveCategoryConfig && pveCategoryConfig.HasCategory(envCategoryID))
				{
					Print("[TrackingMod] Environmental death category " + envCategoryID + " is a PVE category - counted as PVE death");
				}
			}
		}
		
		SavePlayerData(playerData, playerID);
	}
	
	void AddKill(string playerID, string playerName, float range, string victimID = "", Object victimEntity = null, int killCause = -1)
	{
		string victimClassName;
		#ifdef NinjinsPvPPvE
		string currentZoneType;
		#endif
		PlayerDeathData playerData;
		PVPCategoryConfig pvpCategoryConfig;
		array<string> matchingCategories;
		int i;
		string categoryID;
		
		if (playerID == victimID)
		{
			Print("[TrackingMod] Self-kill detected - kill not recorded (Player: " + playerID + ")");
			return;
		}
		
		if (g_TrackingModConfig && g_TrackingModConfig.DisablePVPLeaderboard)
		{
			Print("[TrackingMod] PVP kill tracking disabled - kill not recorded (Killer: " + playerID + ", Victim: " + victimID + ")");
			return;
		}
		
		if (killCause != -1 && g_TrackingModConfig && g_TrackingModConfig.ShouldExcludeKillCause(killCause))
		{
			Print("[TrackingMod] Kill excluded by config - cause: " + killCause.ToString() + " (Killer: " + playerID + ", Victim: " + victimID + ")");
			return;
		}
		
		victimClassName = "PlayerBase";
		if (victimEntity)
			victimClassName = victimEntity.GetType();
		
		#ifdef NinjinsPvPPvE
		if (g_TrackingModConfig && g_TrackingModConfig.ExcludePVPKillWhenBothInPVE && victimID != "")
		{
			if (TrackingModZoneUtils.IsPlayerInPVEZone(playerID) && TrackingModZoneUtils.IsPlayerInPVEZone(victimID))
			{
				Print("[TrackingMod] PVP kill excluded - both players in PVE zone (Killer: " + playerID + ", Victim: " + victimID + ")");
				return;
			}
		}
		
		if (g_TrackingModConfig)
		{
			currentZoneType = "";
			if (TrackingModZoneUtils.IsPlayerInPVPZone(playerID))
				currentZoneType = "pvp";
			else if (TrackingModZoneUtils.IsPlayerInPVEZone(playerID))
				currentZoneType = "pve";
			else if (TrackingModZoneUtils.IsPlayerInVisualZone(playerID))
				currentZoneType = "visual";
			else if (TrackingModZoneUtils.IsPlayerInRaidZone(playerID))
				currentZoneType = "raid";
			
			if (currentZoneType != "" && g_TrackingModConfig.ShouldExcludeKillInZone(currentZoneType, victimClassName, victimEntity, playerID))
			{
				Print("[TrackingMod] Player kill excluded - zone type: " + currentZoneType + ", class: " + victimClassName + " (Killer: " + playerID + ")");
				return;
			}
		}
		#endif
		
		playerData = GetPlayerData(playerID, playerName);
		playerData.PlayerName = playerName;
		
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		matchingCategories = pvpCategoryConfig.GetCategoryIDsForClass(victimClassName, victimEntity);
		
		if (matchingCategories.Count() > 0)
		{
			for (i = 0; i < matchingCategories.Count(); i++)
			{
				categoryID = matchingCategories[i];
				playerData.AddCategoryKill(categoryID, range, 1);
				Print("[TrackingMod] PVP kill added to category: " + categoryID + " (killer: " + playerID + ", victim class: " + victimClassName + ")");
			}
		}
		else
		{
			categoryID = pvpCategoryConfig.GetPrimaryPlayerCategoryID();
			playerData.AddCategoryKill(categoryID, range, 1);
			Print("[TrackingMod] PVP kill added to default PVP category: " + categoryID + " (killer: " + playerID + ", victim class: " + victimClassName + " - no match found)");
		}
		
		SavePlayerData(playerData, playerID);
	}
	
	void AddZombieKill(string playerID, string playerName, float range, string className = "", Object entity = null, int killCause = -1)
	{
		#ifdef NinjinsPvPPvE
		string currentZoneType;
		#endif
		PlayerDeathData playerData;
		
		if (g_TrackingModConfig && g_TrackingModConfig.DisablePVELeaderboard)
			return;
		
		if (g_TrackingModConfig && !g_TrackingModConfig.ShouldTrackPVEKill(playerID))
			return;
		
		if (killCause != -1 && g_TrackingModConfig && g_TrackingModConfig.ShouldExcludeKillCause(killCause))
		{
			Print("[TrackingMod] Zombie kill excluded by config - cause: " + killCause.ToString() + " (Player: " + playerID + ")");
			return;
		}
		
		#ifdef NinjinsPvPPvE
		if (g_TrackingModConfig)
		{
			currentZoneType = "";
			if (TrackingModZoneUtils.IsPlayerInPVPZone(playerID))
				currentZoneType = "pvp";
			else if (TrackingModZoneUtils.IsPlayerInPVEZone(playerID))
				currentZoneType = "pve";
			else if (TrackingModZoneUtils.IsPlayerInVisualZone(playerID))
				currentZoneType = "visual";
			else if (TrackingModZoneUtils.IsPlayerInRaidZone(playerID))
				currentZoneType = "raid";
			
			if (currentZoneType != "" && className != "" && g_TrackingModConfig.ShouldExcludeKillInZone(currentZoneType, className, entity, playerID))
			{
				Print("[TrackingMod] Zombie kill excluded - zone type: " + currentZoneType + ", class: " + className + " (Player: " + playerID + ")");
				return;
			}
		}
		#endif
		
		playerData = GetPlayerData(playerID, playerName);
		playerData.PlayerName = playerName;
		
		if (className != "")
		{
			AddKillToCategories(playerData, className, range, entity);
		}
		
		SavePlayerData(playerData, playerID);
	}
	
	void AddAnimalKill(string playerID, string playerName, float range, string className = "", Object entity = null, int killCause = -1)
	{
		#ifdef NinjinsPvPPvE
		string currentZoneType;
		#endif
		PlayerDeathData playerData;
		
		if (g_TrackingModConfig && g_TrackingModConfig.DisablePVELeaderboard)
			return;
		
		if (g_TrackingModConfig && !g_TrackingModConfig.ShouldTrackPVEKill(playerID))
			return;
		
		if (killCause != -1 && g_TrackingModConfig && g_TrackingModConfig.ShouldExcludeKillCause(killCause))
		{
			Print("[TrackingMod] Animal kill excluded by config - cause: " + killCause.ToString() + " (Player: " + playerID + ")");
			return;
		}
		
		#ifdef NinjinsPvPPvE
		if (g_TrackingModConfig)
		{
			currentZoneType = "";
			if (TrackingModZoneUtils.IsPlayerInPVPZone(playerID))
				currentZoneType = "pvp";
			else if (TrackingModZoneUtils.IsPlayerInPVEZone(playerID))
				currentZoneType = "pve";
			else if (TrackingModZoneUtils.IsPlayerInVisualZone(playerID))
				currentZoneType = "visual";
			else if (TrackingModZoneUtils.IsPlayerInRaidZone(playerID))
				currentZoneType = "raid";
			
			if (currentZoneType != "" && className != "" && g_TrackingModConfig.ShouldExcludeKillInZone(currentZoneType, className, entity, playerID))
			{
				Print("[TrackingMod] Animal kill excluded - zone type: " + currentZoneType + ", class: " + className + " (Player: " + playerID + ")");
				return;
			}
		}
		#endif
		
		playerData = GetPlayerData(playerID, playerName);
		playerData.PlayerName = playerName;
		
		if (className != "")
		{
			AddKillToCategories(playerData, className, range, entity);
		}
		
		SavePlayerData(playerData, playerID);
	}
	
	void AddAIKill(string playerID, string playerName, float range, string className = "", Object entity = null, int killCause = -1)
	{
		#ifdef NinjinsPvPPvE
		string currentZoneType;
		#endif
		PlayerDeathData playerData;
		
		if (g_TrackingModConfig && g_TrackingModConfig.DisablePVELeaderboard)
			return;
		
		if (g_TrackingModConfig && !g_TrackingModConfig.ShouldTrackPVEKill(playerID))
			return;
		
		if (killCause != -1 && g_TrackingModConfig && g_TrackingModConfig.ShouldExcludeKillCause(killCause))
		{
			Print("[TrackingMod] AI kill excluded by config - cause: " + killCause.ToString() + " (Player: " + playerID + ")");
			return;
		}
		
		#ifdef NinjinsPvPPvE
		if (g_TrackingModConfig)
		{
			currentZoneType = "";
			if (TrackingModZoneUtils.IsPlayerInPVPZone(playerID))
				currentZoneType = "pvp";
			else if (TrackingModZoneUtils.IsPlayerInPVEZone(playerID))
				currentZoneType = "pve";
			else if (TrackingModZoneUtils.IsPlayerInVisualZone(playerID))
				currentZoneType = "visual";
			else if (TrackingModZoneUtils.IsPlayerInRaidZone(playerID))
				currentZoneType = "raid";
			
			if (currentZoneType != "" && className != "" && g_TrackingModConfig.ShouldExcludeKillInZone(currentZoneType, className, entity, playerID))
			{
				Print("[TrackingMod] AI kill excluded - zone type: " + currentZoneType + ", class: " + className + " (Player: " + playerID + ")");
				return;
			}
		}
		#endif
		
		playerData = GetPlayerData(playerID, playerName);
		playerData.PlayerName = playerName;
		
		if (className != "")
		{
			AddKillToCategories(playerData, className, range, entity);
		}
		
		SavePlayerData(playerData, playerID);
	}
	
	void AddKillToCategories(PlayerDeathData playerData, string className, float range, Object entity = null)
	{
		PVECategoryConfig categoryConfig;
		array<ref CategoryMatch> matches;
		int i;
		CategoryMatch match;
		
		categoryConfig = PVECategoryConfig.GetInstance();
		matches = categoryConfig.GetCategoryMatchesForClass(className, entity);
		
		if (matches.Count() == 0)
		{
			Print("[TrackingMod] No matching category found for class: " + className);
			return;
		}
		
		Print("[TrackingMod] Found " + matches.Count().ToString() + " matching categories for class: " + className);
		
		for (i = 0; i < matches.Count(); i++)
		{
			match = matches[i];
			if (match)
			{
				playerData.AddCategoryKill(match.CategoryID, range, match.Multiplier);
				Print("[TrackingMod] Added kill to category: " + match.CategoryID + " with multiplier: " + match.Multiplier.ToString());

			}
		}
		
		playerData.UpdatePVEPoints();
	}
	
	void CleanupOldPlayerFiles(int maxAgeDays)
	{
		int deletedCount;
		int i;
		array<ref CF_File> playerFiles;
		string jsonPattern;
		CF_File playerFile;
		string filePath;
		string fileName;
		string playerID;
		PlayerDeathData playerData;
		CF_Date currentDate;
		CF_Date lastLoginDate;
		int daysSinceLogin;
		int currentTimestamp;
		int lastLoginTimestamp;
		int secondsDiff;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		if (maxAgeDays <= 0)
			return;
		
		bool useUTC;
		if (g_TrackingModConfig)
			useUTC = g_TrackingModConfig.UseUTCForDates;
		else
			useUTC = false;
		currentDate = CF_Date.Now(useUTC);
		deletedCount = 0;
		playerFiles = new array<ref CF_File>();
		jsonPattern = TRACKING_MOD_PLAYERS_DIR + "*.json";
		
		if (!CF_Directory.GetFiles(jsonPattern, playerFiles, 2))
			return;
		
		TrackingMod.LogDebug(string.Format("[Cleanup] Checking %1 player files for cleanup (max age: %2 days)", playerFiles.Count(), maxAgeDays));
		
		for (i = 0; i < playerFiles.Count(); i++)
		{
			playerFile = playerFiles.Get(i);
			filePath = playerFile.GetFullPath();
			fileName = playerFile.GetFileName();
			
			if (fileName && fileName.Length() > 5)
			{
				playerID = fileName;
				playerID = playerID.Substring(0, playerID.Length() - 5);
				
				if (playerID != "" && FileExist(filePath))
				{
					playerData = new PlayerDeathData();
					JsonFileLoader<PlayerDeathData>.JsonLoadFile(filePath, playerData);
					
					if (playerData && playerData.PlayerID != "")
					{
						if (playerData.LastLoginDate == "")
						{
							playerData.LastLoginDate = currentDate.ToString(CF_Date.DATETIME);
							JsonFileLoader<PlayerDeathData>.JsonSaveFile(filePath, playerData);
							TrackingMod.LogDebug(string.Format("[Cleanup] Updated LastLoginDate for player %1 (was empty)", playerID));
							continue;
						}
						
						lastLoginDate = ParseDateString(playerData.LastLoginDate);
						if (!lastLoginDate)
						{
							playerData.LastLoginDate = currentDate.ToString(CF_Date.DATETIME);
							JsonFileLoader<PlayerDeathData>.JsonSaveFile(filePath, playerData);
							TrackingMod.LogDebug(string.Format("[Cleanup] Updated LastLoginDate for player %1 (invalid format)", playerID));
							continue;
						}
						
						currentTimestamp = currentDate.GetTimestamp();
						lastLoginTimestamp = lastLoginDate.GetTimestamp();
						secondsDiff = currentTimestamp - lastLoginTimestamp;
						daysSinceLogin = secondsDiff / 86400;
						
						if (daysSinceLogin > maxAgeDays)
						{
							if (playerFile.Delete())
							{
								if (m_PlayerDataMap && m_PlayerDataMap.Contains(playerID))
								{
									m_PlayerDataMap.Remove(playerID);
								}
								deletedCount++;
								TrackingMod.LogInfo(string.Format("[Cleanup] Deleted old player file: %1 (last login: %2, %3 days ago)", playerID, playerData.LastLoginDate, daysSinceLogin));
							}
							else
							{
								TrackingMod.LogWarning(string.Format("[Cleanup] Failed to delete file: %1", filePath));
							}
						}
					}
				}
			}
		}
		
		if (deletedCount > 0)
		{
			TrackingMod.LogInfo(string.Format("[Cleanup] Cleanup complete: Deleted %1 old player files", deletedCount));
		}
		else
		{
			TrackingMod.LogDebug("[Cleanup] No old player files to delete");
		}
	}
	
	static void CleanupOldFiles()
	{
		int maxAgeDays;
		
		if (!g_TrackingModData || !g_TrackingModConfig)
			return;
		
		maxAgeDays = g_TrackingModConfig.DeletePlayerFilesOlderThanDays;
		if (maxAgeDays <= 0)
			return;
		
		g_TrackingModData.CleanupOldPlayerFiles(maxAgeDays);
	}
	
	static CF_Date ParseDateString(string dateString)
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		TStringArray parts;
		TStringArray dateParts;
		TStringArray timeParts;
		string datePart;
		string timePart;
		
		if (dateString == "")
			return null;
		
		parts = new TStringArray();
		dateString.Split(" ", parts);
		
		if (parts.Count() < 2)
			return null;
		
		datePart = parts[0];
		timePart = parts[1];
		
		dateParts = new TStringArray();
		datePart.Split("-", dateParts);
		
		if (dateParts.Count() < 3)
			return null;
		
		year = dateParts[0].ToInt();
		month = dateParts[1].ToInt();
		day = dateParts[2].ToInt();
		
		timeParts = new TStringArray();
		timePart.Split(":", timeParts);
		
		if (timeParts.Count() < 3)
			return null;
		
		hour = timeParts[0].ToInt();
		minute = timeParts[1].ToInt();
		second = timeParts[2].ToInt();
		
		return CF_Date.CreateDateTime(year, month, day, hour, minute, second);
	}
	
}

class TrackingModColumnConfig
{
	string ColumnID;
	string DisplayName;
	bool Visible;
	int SortOrder;

	void TrackingModColumnConfig()
	{
		ColumnID = "";
		DisplayName = "";
		Visible = true;
		SortOrder = 0;
	}
}

class TrackingModColumnSort
{
	static string s_NJN_CurrentSortColumn;
	static bool s_NJN_SortAscending;

	static void SetSort(string columnID)
	{
		if (s_NJN_CurrentSortColumn == columnID)
		{
			s_NJN_SortAscending = !s_NJN_SortAscending;
		}
		else
		{
			s_NJN_CurrentSortColumn = columnID;
			s_NJN_SortAscending = false;
		}
	}

	static string GetCurrentColumn()
	{
		return s_NJN_CurrentSortColumn;
	}

	static bool IsAscending()
	{
		return s_NJN_SortAscending;
	}

	static void ClearSort()
	{
		s_NJN_CurrentSortColumn = "";
		s_NJN_SortAscending = false;
	}

	// Resolves a category value, falling back to summing the legacy player
	// category keys when the requested ID is the primary player category.
	// Mirrors what the PvP entry rows display.
	static int GetPVPCategoryValue(map<string, int> categoryMap, string categoryID)
	{
		PVPCategoryConfig pvpCategoryConfig;
		string primaryCategoryID;
		int totalValue;

		if (!categoryMap)
			return 0;

		if (categoryMap.Contains(categoryID))
			return categoryMap.Get(categoryID);

		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return 0;

		primaryCategoryID = pvpCategoryConfig.GetPrimaryPlayerCategoryID();
		if (categoryID != primaryCategoryID)
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

	static int GetTotalPVPKills(TrackingModLeaderboardPlayerData entry)
	{
		PVPCategoryConfig pvpCategoryConfig;
		array<ref PVPCategory> categories;
		map<string, bool> processedCategories;
		PVPCategory category;
		string categoryID;
		int totalKills;
		int i;

		if (!entry || !entry.categoryKills)
			return 0;

		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return GetPVPCategoryValue(entry.categoryKills, "Players");

		categories = pvpCategoryConfig.GetCategories();
		processedCategories = new map<string, bool>();
		totalKills = 0;
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories.Get(i);
			if (!category || category.CategoryID == "")
				continue;

			categoryID = category.CategoryID;
			if (processedCategories.Contains(categoryID))
				continue;
			processedCategories.Set(categoryID, true);
			totalKills += GetPVPCategoryValue(entry.categoryKills, categoryID);
		}

		return totalKills;
	}

	static int GetLongestPVPRange(TrackingModLeaderboardPlayerData entry)
	{
		PVPCategoryConfig pvpCategoryConfig;
		array<ref PVPCategory> categories;
		map<string, bool> processedCategories;
		PVPCategory category;
		string categoryID;
		int longestRange;
		int categoryRange;
		int i;

		if (!entry || !entry.categoryLongestRanges)
			return 0;

		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return GetPVPCategoryValue(entry.categoryLongestRanges, "Players");

		categories = pvpCategoryConfig.GetCategories();
		processedCategories = new map<string, bool>();
		longestRange = 0;
		for (i = 0; i < categories.Count(); i++)
		{
			category = categories.Get(i);
			if (!category || category.CategoryID == "")
				continue;

			categoryID = category.CategoryID;
			if (processedCategories.Contains(categoryID))
				continue;
			processedCategories.Set(categoryID, true);
			categoryRange = GetPVPCategoryValue(entry.categoryLongestRanges, categoryID);
			if (categoryRange > longestRange)
				longestRange = categoryRange;
		}

		return longestRange;
	}

	// Longest range over all non-PvP categories, matching the PvE entry rows.
	static int GetLongestPVERange(TrackingModLeaderboardPlayerData entry)
	{
		PVPCategoryConfig pvpCategoryConfig;
		string categoryID;
		int longestRange;
		int categoryRange;
		int i;

		if (!entry || !entry.categoryLongestRanges)
			return 0;

		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		longestRange = 0;
		for (i = 0; i < entry.categoryLongestRanges.Count(); i++)
		{
			categoryID = entry.categoryLongestRanges.GetKey(i);
			if (pvpCategoryConfig && pvpCategoryConfig.HasCategory(categoryID))
				continue;

			categoryRange = entry.categoryLongestRanges.GetElement(i);
			if (categoryRange > longestRange)
				longestRange = categoryRange;
		}

		return longestRange;
	}

	// Kills per death; with zero deaths the kill count itself, like the K/D
	// column displays. Float division on purpose - 5 kills / 2 deaths is 2.5.
	static float GetKDRatio(TrackingModLeaderboardPlayerData entry)
	{
		int kills;
		float ratio;

		if (!entry)
			return 0.0;

		kills = TrackingModColumnSort.GetTotalPVPKills(entry);
		ratio = kills;
		if (entry.deathCount > 0)
			ratio = ratio / entry.deathCount;

		return ratio;
	}

	// True when the active column holds a float rather than an int.
	static bool IsFloatColumn(string columnID)
	{
		if (columnID == "Accuracy")
			return true;
		if (columnID == "DistanceTravelled")
			return true;
		if (columnID == "DistanceOnFoot")
			return true;
		if (columnID == "DistanceInVehicle")
			return true;
		if (columnID == "KDRatio")
			return true;
		return false;
	}

	// Integer key for the active column. Unknown columns yield 0 for every row,
	// which, with a stable sort, leaves the list untouched, matching what the
	// old comparison did when it recognised no column.
	static int GetIntColumnValue(TrackingModLeaderboardPlayerData entry, string columnID)
	{
		if (!entry)
			return 0;

		if (columnID == "PvEPoints")
			return entry.pvePoints;
		if (columnID == "PvPPoints")
			return entry.pvpPoints;
		if (columnID == "Deaths")
			return entry.deathCount;
		if (columnID == "ShotsFired")
			return entry.shotsFired;
		if (columnID == "ShotsHit")
			return entry.shotsHit;
		if (columnID == "Headshots")
			return entry.headshots;
		if (columnID == "PlayTime")
			return entry.playTimeSeconds;
		if (columnID == "Reputation")
			return entry.hardlineReputation;
		if (columnID == "TotalDeaths")
			return entry.totalDeaths;
		if (columnID == "Suicides")
			return entry.suicides;
		if (columnID == "LongestRange")
			return TrackingModColumnSort.GetLongestPVERange(entry);
		if (columnID == "PvPLongestRange")
			return TrackingModColumnSort.GetLongestPVPRange(entry);
		if (columnID == "PlayerKills")
			return TrackingModColumnSort.GetTotalPVPKills(entry);

		return 0;
	}

	static float GetFloatColumnValue(TrackingModLeaderboardPlayerData entry, string columnID)
	{
		if (!entry)
			return 0.0;

		if (columnID == "Accuracy")
			return entry.accuracy;
		if (columnID == "DistanceTravelled")
			return entry.distanceTravelled;
		if (columnID == "DistanceOnFoot")
			return entry.distanceOnFoot;
		if (columnID == "DistanceInVehicle")
			return entry.distanceInVehicle;
		if (columnID == "KDRatio")
			return TrackingModColumnSort.GetKDRatio(entry);

		return 0.0;
	}

	static void SortPlayerList(array<ref TrackingModLeaderboardPlayerData> playerList)
	{
		array<int> intKeys;
		array<float> floatKeys;
		array<string> stringKeys;
		array<int> order;
		TrackingModLeaderboardPlayerData entry;
		string column;
		bool sortAscending;
		int i;
		int count;

		if (!playerList || playerList.Count() <= 1)
			return;

		column = s_NJN_CurrentSortColumn;
		if (column == "")
			return;

		count = playerList.Count();
		order = new array<int>();

		// SetSort() starts every fresh column descending (flag == false) and
		// toggles the flag on repeat clicks; the key sort follows it directly.
		sortAscending = s_NJN_SortAscending;

		if (column == "Name")
		{
			stringKeys = new array<string>();
			for (i = 0; i < count; i++)
			{
				entry = playerList.Get(i);
				if (entry)
					stringKeys.Insert(entry.playerName);
				else
					stringKeys.Insert("");
			}
			TrackingModSort.BuildOrderByString(stringKeys, sortAscending, order);
		}
		else if (TrackingModColumnSort.IsFloatColumn(column))
		{
			floatKeys = new array<float>();
			for (i = 0; i < count; i++)
			{
				entry = playerList.Get(i);
				floatKeys.Insert(TrackingModColumnSort.GetFloatColumnValue(entry, column));
			}
			TrackingModSort.BuildOrderByFloat(floatKeys, sortAscending, order);
		}
		else
		{
			intKeys = new array<int>();
			for (i = 0; i < count; i++)
			{
				entry = playerList.Get(i);
				intKeys.Insert(TrackingModColumnSort.GetIntColumnValue(entry, column));
			}
			TrackingModSort.BuildOrderByInt(intKeys, sortAscending, order);
		}

		TrackingModSort.ApplyOrderLeaderboardPlayers(playerList, order);
	}
}

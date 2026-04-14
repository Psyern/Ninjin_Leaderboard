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

	static int ComparePlayerData(TrackingModLeaderboardPlayerData a, TrackingModLeaderboardPlayerData b)
	{
		int valA;
		int valB;
		float fValA;
		float fValB;
		int result;

		if (!a || !b)
			return 0;

		result = 0;
		if (s_NJN_CurrentSortColumn == "Name")
		{
			result = a.playerName.Compare(b.playerName);
		}
		else if (s_NJN_CurrentSortColumn == "PvEPoints")
		{
			valA = a.pvePoints;
			valB = b.pvePoints;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "PvPPoints")
		{
			valA = a.pvpPoints;
			valB = b.pvpPoints;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "Deaths")
		{
			valA = a.deathCount;
			valB = b.deathCount;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "ShotsFired")
		{
			valA = a.shotsFired;
			valB = b.shotsFired;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "ShotsHit")
		{
			valA = a.shotsHit;
			valB = b.shotsHit;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "Headshots")
		{
			valA = a.headshots;
			valB = b.headshots;
			if (valA > valB)
				result = 1;
			else if (valA < valB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "Accuracy")
		{
			fValA = a.accuracy;
			fValB = b.accuracy;
			if (fValA > fValB)
				result = 1;
			else if (fValA < fValB)
				result = -1;
		}
		else if (s_NJN_CurrentSortColumn == "DistanceTravelled")
		{
			fValA = a.distanceTravelled;
			fValB = b.distanceTravelled;
			if (fValA > fValB)
				result = 1;
			else if (fValA < fValB)
				result = -1;
		}

		if (s_NJN_SortAscending)
			return result;
		else
			return -result;
	}

	static void SortPlayerList(array<ref TrackingModLeaderboardPlayerData> playerList)
	{
		int i;
		int j;
		int compareResult;

		if (!playerList || playerList.Count() <= 1)
			return;
		if (s_NJN_CurrentSortColumn == "")
			return;

		for (i = 0; i < playerList.Count() - 1; i++)
		{
			for (j = i + 1; j < playerList.Count(); j++)
			{
				compareResult = ComparePlayerData(playerList.Get(i), playerList.Get(j));
				if (compareResult < 0)
				{
					playerList.SwapItems(i, j);
				}
			}
		}
	}
}

class TrackingModPvPPlayerEntry: ScriptView
{
	protected ref TrackingModPvPPlayerEntryController m_EntryController;
	protected ref TrackingModLeaderboardPlayerData m_PlayerData;
	protected int m_Position;
	protected bool m_ShowOnlineStatus = true;
	protected string m_SurvivorIconPathMale;
	protected string m_SurvivorIconPathFemale;
	protected Widget player_online_indicator;
	protected ButtonWidget claim_reward_button;
	protected ImageWidget Survivor_Female;

	protected int GetPVPCategoryValue(map<string, int> categoryMap, string categoryID)
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

	protected int GetTotalPVPKills()
	{
		PVPCategoryConfig pvpCategoryConfig;
		array<ref PVPCategory> categories;
		ref map<string, bool> processedCategories;
		PVPCategory category;
		string categoryID;
		int totalKills;
		int i;
		
		if (!m_PlayerData || !m_PlayerData.categoryKills)
			return 0;
		
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return GetPVPCategoryValue(m_PlayerData.categoryKills, "Players");
		
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
			totalKills += GetPVPCategoryValue(m_PlayerData.categoryKills, categoryID);
		}
		
		return totalKills;
	}

	protected int GetLongestPVPRange()
	{
		PVPCategoryConfig pvpCategoryConfig;
		array<ref PVPCategory> categories;
		ref map<string, bool> processedCategories;
		PVPCategory category;
		string categoryID;
		int longestRange;
		int categoryRange;
		int i;
		
		if (!m_PlayerData || !m_PlayerData.categoryLongestRanges)
			return 0;
		
		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		if (!pvpCategoryConfig)
			return GetPVPCategoryValue(m_PlayerData.categoryLongestRanges, "Players");
		
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
			categoryRange = GetPVPCategoryValue(m_PlayerData.categoryLongestRanges, categoryID);
			if (categoryRange > longestRange)
				longestRange = categoryRange;
		}
		
		return longestRange;
	}
	
	void TrackingModPvPPlayerEntry(TrackingModLeaderboardPlayerData playerData, int position)
	{
		string positionStr;
		int playerKills;
		float kdRatio;
		string kdStr;
		int dotPos;
		int decimals;
		int longestRange;
		
		m_PlayerData = playerData;
		m_Position = position;
		m_EntryController = TrackingModPvPPlayerEntryController.Cast(m_Controller);
		
		if (m_PlayerData)
		{
			positionStr = position.ToString();
			m_EntryController.PlayerName = m_PlayerData.playerName;
			m_EntryController.PlayerPosition = positionStr;
			m_EntryController.PVPPoints = m_PlayerData.pvpPoints.ToString();
			
			playerKills = GetTotalPVPKills();
			
			m_EntryController.Kills = playerKills.ToString();
			m_EntryController.Deaths = m_PlayerData.deathCount.ToString();
			
			kdRatio = 0.0;
			if (m_PlayerData.deathCount > 0)
				kdRatio = playerKills / m_PlayerData.deathCount;
			else if (playerKills > 0)
				kdRatio = playerKills;
			kdRatio = Math.Round(kdRatio * 100.0) / 100.0;
			kdStr = kdRatio.ToString();
			dotPos = kdStr.IndexOf(".");
			if (dotPos == -1)
			{
				kdStr = kdStr + ".00";
			}
			else
			{
				decimals = kdStr.Length() - dotPos - 1;
				if (decimals == 0)
					kdStr = kdStr + "00";
				else if (decimals == 1)
					kdStr = kdStr + "0";
				else if (decimals > 2)
					kdStr = kdStr.Substring(0, dotPos + 3);
			}
			m_EntryController.KDRatio = kdStr;
			
			longestRange = GetLongestPVPRange();
			
			m_EntryController.LongestRange = longestRange.ToString() + " m";
			
			m_EntryController.NotifyPropertiesChanged({"PlayerName", "PlayerPosition", "PVPPoints", "Kills", "Deaths", "KDRatio", "LongestRange"});
		}
	}
	
	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		
		if (m_EntryController)
		{
			m_EntryController.PlayerPosition = m_Position.ToString();
			m_EntryController.NotifyPropertiesChanged({"PlayerPosition"});
		}
		
		InitializeWidgets();
		LoadIconImage();
	}
	
	void InitializeWidgets()
	{
		string imagePath;
		
		if (!GetLayoutRoot())
			return;
		
		player_online_indicator = GetLayoutRoot().FindAnyWidget("player_online_indicator");
		claim_reward_button = ButtonWidget.Cast(GetLayoutRoot().FindAnyWidget("claim_reward_button"));
		Survivor_Female = ImageWidget.Cast(GetLayoutRoot().FindAnyWidget("Survivor_Female"));
		
		if (claim_reward_button)
		{
			UpdateClaimRewardButtonVisibility();
		}
		
		if (Survivor_Female && m_PlayerData)
		{
			imagePath = m_SurvivorIconPathFemale;
			if (m_SurvivorIconPathFemale == "")
				imagePath = "Ninjins_LeaderBoard/gui/icons/survivor/female.edds";
			if (m_PlayerData.survivorType == "Male")
			{
				imagePath = m_SurvivorIconPathMale;
				if (m_SurvivorIconPathMale == "")
					imagePath = "Ninjins_LeaderBoard/gui/icons/survivor/male.edds";
			}
			Survivor_Female.LoadImageFile(0, imagePath);
		}
		
		if (player_online_indicator)
		{
			player_online_indicator.Show(m_ShowOnlineStatus);
		}
		
		if (m_ShowOnlineStatus)
			UpdateOnlineIndicatorColor();

		HighlightIfCurrentPlayer();
	}

	void HighlightIfCurrentPlayer()
	{
		PlayerBase highlightPlayer;
		PlayerIdentity highlightIdentity;
		string highlightPlayerID;
		Widget rootWidget;

		if (!m_PlayerData)
			return;

		highlightPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!highlightPlayer)
			return;

		highlightIdentity = highlightPlayer.GetIdentity();
		if (!highlightIdentity)
			return;

		highlightPlayerID = highlightIdentity.GetPlainId();
		if (highlightPlayerID == m_PlayerData.playerID)
		{
			rootWidget = GetLayoutRoot();
			if (rootWidget)
			{
				if (g_TrackingModStyleConfig && g_TrackingModStyleConfig.CurrentPlayerHighlightColor)
					rootWidget.SetColor(g_TrackingModStyleConfig.CurrentPlayerHighlightColor.ToARGB());
				else
					rootWidget.SetColor(ARGB(80, 255, 215, 0));
			}
		}
	}

	void LoadIconImage()
	{
		string imagePath;
		
		if (!Survivor_Female && GetLayoutRoot())
			Survivor_Female = ImageWidget.Cast(GetLayoutRoot().FindAnyWidget("Survivor_Female"));
		
		if (Survivor_Female && m_PlayerData)
		{
			imagePath = m_SurvivorIconPathFemale;
			if (m_SurvivorIconPathFemale == "")
				imagePath = "Ninjins_LeaderBoard/gui/icons/survivor/female.edds";
			if (m_PlayerData.survivorType == "Male")
			{
				imagePath = m_SurvivorIconPathMale;
				if (m_SurvivorIconPathMale == "")
					imagePath = "Ninjins_LeaderBoard/gui/icons/survivor/male.edds";
			}
			Survivor_Female.LoadImageFile(0, imagePath);
		}
	}
	
	void SetSurvivorIconPaths(string malePath, string femalePath)
	{
		m_SurvivorIconPathMale = malePath;
		m_SurvivorIconPathFemale = femalePath;
	}
	
	void UpdateOnlineIndicatorColor()
	{
		int colorToSet;
		
		if (!player_online_indicator)
		{
			player_online_indicator = GetLayoutRoot().FindAnyWidget("player_online_indicator");
		}
		
		if (player_online_indicator && m_PlayerData)
		{
			if (m_PlayerData.isOnline == 1)
			{
				colorToSet = ARGB(255, 0, 200, 0);
			}
			else
			{
				colorToSet = ARGB(255, 200, 0, 0);
			}
			
			player_online_indicator.SetColor(colorToSet);
			player_online_indicator.Update();
		}
	}
	
	void SetShowOnlineStatus(bool show)
	{
		m_ShowOnlineStatus = show;
		
		if (player_online_indicator)
		{
			player_online_indicator.Show(m_ShowOnlineStatus);
			if (m_ShowOnlineStatus)
				UpdateOnlineIndicatorColor();
		}
	}
	
	void OnClaimRewardButtonClick()
	{
		PlayerBase player;
		PlayerIdentity identity;
		string currentPlayerID;
		bool shouldShow;
		
		Print("[TrackingMod UI] OnClaimRewardButtonClick called (PvP)");
		
		if (!m_PlayerData)
		{
			Print("[TrackingMod UI] ERROR: m_PlayerData is null!");
			return;
		}
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			Print("[TrackingMod UI] ERROR: Player is null!");
			return;
		}
		
		identity = player.GetIdentity();
		if (!identity)
		{
			Print("[TrackingMod UI] ERROR: PlayerIdentity is null!");
			return;
		}
		
		currentPlayerID = identity.GetPlainId();
		Print("[TrackingMod UI] Current Player ID: " + currentPlayerID + ", Entry Player ID: " + m_PlayerData.playerID + ", Pending Rewards: " + m_PlayerData.pendingRewards.ToString());
		
		shouldShow = (currentPlayerID == m_PlayerData.playerID && m_PlayerData.pendingRewards > 0);
		
		if (!shouldShow)
		{
			Print("[TrackingMod UI] Button click ignored - shouldShow is false");
			return;
		}
		
		Print("[TrackingMod UI] Sending ClaimTrackingModReward RPC to server...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ClaimTrackingModReward", null, true, identity);
		Print("[TrackingMod UI] ClaimTrackingModReward RPC sent");
	}
	
	TrackingModLeaderboardPlayerData GetPlayerData()
	{
		return m_PlayerData;
	}
	
	void UpdateClaimRewardButtonVisibility()
	{
		PlayerBase player;
		PlayerIdentity identity;
		string currentPlayerID;
		bool shouldShow;
		
		if (!claim_reward_button || !m_PlayerData)
		{
			if (claim_reward_button)
				claim_reward_button.Show(false);
			return;
		}
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			claim_reward_button.Show(false);
			return;
		}
		
		identity = player.GetIdentity();
		if (!identity)
		{
			claim_reward_button.Show(false);
			return;
		}
		
		currentPlayerID = identity.GetPlainId();
		shouldShow = (currentPlayerID == m_PlayerData.playerID && m_PlayerData.pendingRewards > 0);
		
		claim_reward_button.Show(shouldShow);
	}
	
	override typename GetControllerType()
	{
		return TrackingModPvPPlayerEntryController;
	}
	
	override string GetLayoutFile()
	{
		return "Ninjins_LeaderBoard\\GUI\\layouts\\PvP\\trackingModPvPPlayerEntry_mvc.layout";
	}
}

class TrackingModPlayerEntry: ScriptView
{
	protected ref TrackingModPlayerEntryController m_EntryController;
	protected ref TrackingModLeaderboardPlayerData m_PlayerData;
	protected int m_Position;
	protected bool m_IsExpanded = false;
	protected bool m_ShowOnlineStatus = true;
	protected ref map<string, string> m_CategoryPreviews;
	protected string m_SurvivorIconPathMale;
	protected string m_SurvivorIconPathFemale;
	protected Widget player_online_indicator;
	ButtonWidget player_button;
	ButtonWidget claim_reward_button;
	ImageWidget expand_icon;
	ImageWidget collapse_icon;
	ImageWidget Survivor_Female;
	WrapSpacerWidget kill_categories;
	
	protected int GetLongestPVERange()
	{
		PVPCategoryConfig pvpCategoryConfig;
		string categoryID;
		int longestRange;
		int categoryRange;
		int i;

		if (!m_PlayerData || !m_PlayerData.categoryLongestRanges)
			return 0;

		pvpCategoryConfig = PVPCategoryConfig.GetInstance();
		longestRange = 0;
		for (i = 0; i < m_PlayerData.categoryLongestRanges.Count(); i++)
		{
			categoryID = m_PlayerData.categoryLongestRanges.GetKey(i);
			if (pvpCategoryConfig && pvpCategoryConfig.HasCategory(categoryID))
				continue;

			categoryRange = m_PlayerData.categoryLongestRanges.GetElement(i);
			if (categoryRange > longestRange)
				longestRange = categoryRange;
		}

		return longestRange;
	}

	protected string FormatDistance(float meters)
	{
		int rounded;
		float km;
		string kmStr;
		int dotPos;
		int decimals;

		if (meters < 1000.0)
		{
			rounded = Math.Round(meters);
			return rounded.ToString() + " m";
		}

		km = meters / 1000.0;
		km = Math.Round(km * 10.0) / 10.0;
		kmStr = km.ToString();
		dotPos = kmStr.IndexOf(".");
		if (dotPos == -1)
			kmStr = kmStr + ".0";
		else
		{
			decimals = kmStr.Length() - dotPos - 1;
			if (decimals == 0)
				kmStr = kmStr + "0";
			else if (decimals > 1)
				kmStr = kmStr.Substring(0, dotPos + 2);
		}
		return kmStr + " km";
	}

	void TrackingModPlayerEntry(TrackingModLeaderboardPlayerData playerData, int position)
	{
		int longestRange;

		m_PlayerData = playerData;
		m_Position = position;
		m_EntryController = TrackingModPlayerEntryController.Cast(m_Controller);

		if (m_PlayerData && m_EntryController)
		{
			m_EntryController.PlayerName = m_PlayerData.playerName;
			m_EntryController.PlayerPosition = position.ToString();
			m_EntryController.PVEPoints = m_PlayerData.pvePoints.ToString();
			m_EntryController.ShotsFired = m_PlayerData.shotsFired.ToString();

			longestRange = GetLongestPVERange();
			m_EntryController.LongestRange = longestRange.ToString() + " m";

			m_EntryController.TotalDeaths = m_PlayerData.totalDeaths.ToString();
			m_EntryController.Suicides = m_PlayerData.suicides.ToString();
			m_EntryController.DistanceOnFoot = FormatDistance(m_PlayerData.distanceOnFoot);
			m_EntryController.DistanceInVehicle = FormatDistance(m_PlayerData.distanceInVehicle);

			m_EntryController.NotifyPropertiesChanged({"PlayerName", "PlayerPosition", "PVEPoints", "ShotsFired", "LongestRange", "TotalDeaths", "Suicides", "DistanceOnFoot", "DistanceInVehicle"});
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
		
		player_button = ButtonWidget.Cast(GetLayoutRoot().FindAnyWidget("player_button"));
		claim_reward_button = ButtonWidget.Cast(GetLayoutRoot().FindAnyWidget("claim_reward_button"));
		expand_icon = ImageWidget.Cast(GetLayoutRoot().FindAnyWidget("expand_icon"));
		collapse_icon = ImageWidget.Cast(GetLayoutRoot().FindAnyWidget("collapse_icon"));
		Survivor_Female = ImageWidget.Cast(GetLayoutRoot().FindAnyWidget("Survivor_Female"));
		kill_categories = WrapSpacerWidget.Cast(GetLayoutRoot().FindAnyWidget("kill_categories"));
		player_online_indicator = GetLayoutRoot().FindAnyWidget("player_online_indicator");
		
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
		
		if (kill_categories)
			kill_categories.Show(false);
		
		if (expand_icon)
			expand_icon.Show(true);
		if (collapse_icon)
			collapse_icon.Show(false);
		
		if (player_online_indicator)
			player_online_indicator.Show(m_ShowOnlineStatus);
		
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
	
	void UpdateOnlineIndicatorColor()
	{
		int colorToSet;
		
		if (!player_online_indicator)
			player_online_indicator = GetLayoutRoot().FindAnyWidget("player_online_indicator");
		
		if (player_online_indicator && m_PlayerData)
		{
			if (m_PlayerData.isOnline == 1)
			{
				if (g_TrackingModStyleConfig && g_TrackingModStyleConfig.OnlineIndicatorColor)
					colorToSet = g_TrackingModStyleConfig.OnlineIndicatorColor.ToARGB();
				else
					colorToSet = ARGB(255, 0, 200, 0);
			}
			else
			{
				if (g_TrackingModStyleConfig && g_TrackingModStyleConfig.OfflineIndicatorColor)
					colorToSet = g_TrackingModStyleConfig.OfflineIndicatorColor.ToARGB();
				else
					colorToSet = ARGB(255, 200, 0, 0);
			}
			
			player_online_indicator.SetColor(colorToSet);
			player_online_indicator.Update();
		}
	}
	
	void OnPlayerButtonClick()
	{
		ToggleExpand(!m_IsExpanded);
	}
	
	void OnClaimRewardButtonClick()
	{
		PlayerBase player;
		PlayerIdentity identity;
		string currentPlayerID;
		bool shouldShow;
		
		Print("[TrackingMod UI] OnClaimRewardButtonClick called");
		
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
	
	void SetCategoryPreviews(map<string, string> categoryPreviews)
	{
		m_CategoryPreviews = categoryPreviews;
		if (m_PlayerData && m_PlayerData.categoryKills)
			LoadKillCategories();
	}
	
	void SetSurvivorIconPaths(string malePath, string femalePath)
	{
		m_SurvivorIconPathMale = malePath;
		m_SurvivorIconPathFemale = femalePath;
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
	
	void LoadKillCategories()
	{
		string categoryID;
		int killCount;
		string displayName;
		string previewClassName;
		TrackingModKillCategory category;
		
		if (!m_PlayerData || !m_PlayerData.categoryKills)
			return;
		
		m_EntryController.KillCategories.Clear();
		
		for (int i = 0; i < m_PlayerData.categoryKills.Count(); i++)
		{
			categoryID = m_PlayerData.categoryKills.GetKey(i);
			killCount = m_PlayerData.categoryKills.GetElement(i);
			
			if (categoryID == "Players" || categoryID == "Player")
				continue;
			
			if (killCount > 0)
			{
				displayName = categoryID;
				previewClassName = "";
				
				if (m_CategoryPreviews && m_CategoryPreviews.Contains(categoryID))
					previewClassName = m_CategoryPreviews.Get(categoryID);
				
				category = new TrackingModKillCategory(categoryID, displayName, killCount, previewClassName);
				m_EntryController.KillCategories.Insert(category);
				
				if (previewClassName != "" && previewClassName != string.Empty)
					category.LoadCategoryPreview();
			}
		}
	}
	
	void ToggleExpand(bool toggleState)
	{
		if (kill_categories)
			kill_categories.Show(toggleState);
		
		if (expand_icon)
			expand_icon.Show(!toggleState);
		if (collapse_icon)
			collapse_icon.Show(toggleState);
		
		m_IsExpanded = toggleState;
	}
	
	override typename GetControllerType()
	{
		return TrackingModPlayerEntryController;
	}
	
	override string GetLayoutFile()
	{
		return "Ninjins_LeaderBoard\\GUI\\layouts\\PvE\\trackingModPlayerEntry_mvc.layout";
	}
}

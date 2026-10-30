class TrackingModPvPPlayerEntry: ScriptView
{
	protected ref TrackingModPvPPlayerEntryController m_EntryController;
	protected ref TrackingModLeaderboardPlayerData m_PlayerData;
	protected int m_Position;
	protected bool m_ShowOnlineStatus = true;
	protected bool m_ShowReputation = true;
	protected TextWidget m_ReputationWidget;
	protected string m_SurvivorIconPathMale;
	protected string m_SurvivorIconPathFemale;
	protected Widget player_online_indicator;
	protected ButtonWidget claim_reward_button;
	protected ImageWidget Survivor_Female;

	protected string FormatPlaytime(int totalSeconds)
	{
		int days;
		int hours;
		int minutes;
		int remainder;

		if (totalSeconds <= 0)
			return "0h";

		days = totalSeconds / 86400;
		remainder = totalSeconds - (days * 86400);
		hours = remainder / 3600;
		remainder = remainder - (hours * 3600);
		minutes = remainder / 60;

		if (days > 0)
			return days.ToString() + "d " + hours.ToString() + "h";
		if (hours > 0)
			return hours.ToString() + "h " + minutes.ToString() + "m";
		return minutes.ToString() + "m";
	}

	// Shared with the column sort (TrackingModColumnSort) so the sorted order
	// always matches the displayed values.
	protected int GetTotalPVPKills()
	{
		return TrackingModColumnSort.GetTotalPVPKills(m_PlayerData);
	}

	protected int GetLongestPVPRange()
	{
		return TrackingModColumnSort.GetLongestPVPRange(m_PlayerData);
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
			
			// Float division via the shared helper - the old int division
			// truncated 5 kills / 2 deaths to 2.00 instead of 2.50.
			kdRatio = TrackingModColumnSort.GetKDRatio(m_PlayerData);
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
			m_EntryController.PlayTime = FormatPlaytime(m_PlayerData.playTimeSeconds);
			m_EntryController.Reputation = m_PlayerData.hardlineReputation.ToString();

			m_EntryController.NotifyPropertiesChanged({"PlayerName", "PlayerPosition", "PVPPoints", "Kills", "Deaths", "KDRatio", "LongestRange", "PlayTime", "Reputation"});
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
		m_ReputationWidget = TextWidget.Cast(GetLayoutRoot().FindAnyWidget("player_reputation"));
		if (m_ReputationWidget)
			m_ReputationWidget.Show(m_ShowReputation);
		
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

		highlightPlayer = PlayerBase.Cast(g_Game.GetPlayer());
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
	
	void SetShowReputation(bool show)
	{
		m_ShowReputation = show;
		if (!m_ReputationWidget && GetLayoutRoot())
			m_ReputationWidget = TextWidget.Cast(GetLayoutRoot().FindAnyWidget("player_reputation"));
		if (m_ReputationWidget)
			m_ReputationWidget.Show(show);
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
		
		player = PlayerBase.Cast(g_Game.GetPlayer());
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
		
		player = PlayerBase.Cast(g_Game.GetPlayer());
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

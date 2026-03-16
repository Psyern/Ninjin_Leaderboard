modded class MissionBase
{
	protected bool m_PendingPVEMode;
	protected bool m_PendingPVPMode;
	protected ref TrackingModLeaderboardMenu m_LeaderboardMenu;
	protected ref TrackingModPvPLeaderboardMenu m_PvPLeaderboardMenu;
	protected bool m_RPCRegistered = false;
	
	override void OnUpdate(float timeslice)
	{
		PlayerBase player;
		UAInput pveInput;
		UAInput pvpInput;
		
		super.OnUpdate(timeslice);
		
		if (!m_RPCRegistered)
		{
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "ReceiveTrackingModLeaderboard", this, SingleplayerExecutionType.Client);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "ReceiveTrackingModLeaderboardDenied", this, SingleplayerExecutionType.Client);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", this, SingleplayerExecutionType.Client);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "ReceivePlayerDataUpdate", this, SingleplayerExecutionType.Client);
			TrackingModUI.InitLogFile();
			m_RPCRegistered = true;
		}
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		
		pveInput = GetUApi().GetInputByName("UAOpenTrackingModLeaderboardPVE");
		if (pveInput && pveInput.LocalPress())
		{
			if (m_LeaderboardMenu && m_LeaderboardMenu.IsVisible())
			{
				if (m_LeaderboardMenu)
				{
					TrackingModUI.LogRPC("[Cache] Menu closing, clearing cache");
					m_LeaderboardMenu.ClearCache();
				}
				GetGame().GetMission().GetHud().Show(true);
				m_LeaderboardMenu.Close();
				m_LeaderboardMenu = null;
			}
			else if (!m_LeaderboardMenu)
			{
				if (GetGame().GetUIManager().GetMenu())
					return;
				
				OpenTrackingModLeaderboardPVE();
			}
		}
		
		pvpInput = GetUApi().GetInputByName("UAOpenTrackingModLeaderboardPVP");
		if (pvpInput && pvpInput.LocalPress())
		{
			if (m_PvPLeaderboardMenu && m_PvPLeaderboardMenu.IsVisible())
			{
				GetGame().GetMission().GetHud().Show(true);
				m_PvPLeaderboardMenu.Close();
				m_PvPLeaderboardMenu = null;
			}
			else if (!m_PvPLeaderboardMenu)
			{
				if (GetGame().GetUIManager().GetMenu())
					return;
				
				OpenTrackingModLeaderboardPVP();
			}
		}
		
		if (m_LeaderboardMenu && !m_LeaderboardMenu.IsVisible())
		{
			GetGame().GetMission().GetHud().Show(true);
			m_LeaderboardMenu = null;
		}
		
		if (m_PvPLeaderboardMenu && !m_PvPLeaderboardMenu.IsVisible())
		{
			GetGame().GetMission().GetHud().Show(true);
			m_PvPLeaderboardMenu = null;
		}
	}
	
	void OpenTrackingModLeaderboardPVE()
	{
		PlayerBase player;
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;
		
		m_PendingPVEMode = true;
		m_PendingPVPMode = false;
		
		TrackingModUI.LogRPC("========================================");
		TrackingModUI.LogRPC("Requested data (PVE) via RPC (no local data):");
		TrackingModUI.LogRPC("Mode[PVE]");
		TrackingModUI.LogRPC("PlayerName[" + player.GetIdentity().GetName() + "]");
		TrackingModUI.LogRPC("PlayerID[" + player.GetIdentity().GetPlainId() + "]");
		TrackingModUI.LogRPC("========================================");
		
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "RequestTrackingModLeaderboard", new Param1<int>(1), true, player.GetIdentity());
	}
	
	void OpenTrackingModLeaderboardPVP()
	{
		PlayerBase player;
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;
		
		m_PendingPVPMode = true;
		m_PendingPVEMode = false;
		
		TrackingModUI.LogRPC("========================================");
		TrackingModUI.LogRPC("Requested data (PVP) via RPC (no local data):");
		TrackingModUI.LogRPC("Mode[PVP]");
		TrackingModUI.LogRPC("PlayerName[" + player.GetIdentity().GetName() + "]");
		TrackingModUI.LogRPC("PlayerID[" + player.GetIdentity().GetPlainId() + "]");
		TrackingModUI.LogRPC("========================================");
		
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "RequestTrackingModLeaderboard", new Param1<int>(1), true, player.GetIdentity());
	}

	void ReceiveTrackingModLeaderboardDenied(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> messageParam;
		PlayerBase player;
		string message;
		
		if (type != CallType.Client)
			return;
		
		if (!ctx.Read(messageParam))
			return;
		
		message = "[TrackingMod] Access denied. Admin only.";
		if (messageParam && messageParam.param1 != "")
			message = messageParam.param1;
		
		m_PendingPVEMode = false;
		m_PendingPVPMode = false;
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			player.MessageAction(message);
	}
	
	void ReceiveTrackingModLeaderboard(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<TrackingModLeaderboardData> data;
		TrackingModLeaderboardData leaderboardData;
		PlayerBase player;
		TrackingModLeaderboardPlayerData uiPvePlayerData;
		TrackingModLeaderboardPlayerData uiPvpPlayerData;
		string uiPreviewCatID;
		string uiPreviewClass;
		int uiPveIdx;
		int uiPvpIdx;
		int uiPreviewIdx;
		
		if (type != CallType.Client)
			return;
		
		if (ctx.Read(data))
		{
			leaderboardData = data.param1;
			player = PlayerBase.Cast(GetGame().GetPlayer());
			if (leaderboardData)
			{
				TrackingModUI.LogRPC("========================================");
				TrackingModUI.LogRPC("Received data:");
				TrackingModUI.LogRPC("PlayerOnlineCounter[" + leaderboardData.playerOnlineCounter.ToString() + "]");
				TrackingModUI.LogRPC("ShowPlayerOnlineStatusPVE[" + leaderboardData.showPlayerOnlineStatusPVE.ToString() + "]");
				TrackingModUI.LogRPC("ShowPlayerOnlineStatusPVP[" + leaderboardData.showPlayerOnlineStatusPVP.ToString() + "]");
				TrackingModUI.LogRPC("DisablePVELeaderboard[" + leaderboardData.disablePVELeaderboard.ToString() + "]");
				TrackingModUI.LogRPC("DisablePVPLeaderboard[" + leaderboardData.disablePVPLeaderboard.ToString() + "]");
				TrackingModUI.LogRPC("TopPVEPlayers[" + leaderboardData.topPVEPlayers.Count().ToString() + "]");
				TrackingModUI.LogRPC("TopPVPPlayers[" + leaderboardData.topPVPPlayers.Count().ToString() + "]");
				TrackingModUI.LogRPC("CategoryPreviews[" + leaderboardData.categoryPreviews.Count().ToString() + "]");
				
				if (leaderboardData.topPVEPlayers && leaderboardData.topPVEPlayers.Count() > 0)
				{
					TrackingModUI.LogRPC("PVE Players:");
					for (uiPveIdx = 0; uiPveIdx < leaderboardData.topPVEPlayers.Count(); uiPveIdx++)
					{
						uiPvePlayerData = leaderboardData.topPVEPlayers.Get(uiPveIdx);
						if (uiPvePlayerData)
						{
							TrackingModUI.LogRPC(string.Format("  [%1] PlayerName[%2] PVEPoints[%3] IsOnline[%4] PlayerID[%5]", 
								uiPveIdx + 1, uiPvePlayerData.playerName, uiPvePlayerData.pvePoints, uiPvePlayerData.isOnline, uiPvePlayerData.playerID));
						}
					}
				}
				
				if (leaderboardData.topPVPPlayers && leaderboardData.topPVPPlayers.Count() > 0)
				{
					TrackingModUI.LogRPC("PVP Players:");
					for (uiPvpIdx = 0; uiPvpIdx < leaderboardData.topPVPPlayers.Count(); uiPvpIdx++)
					{
						uiPvpPlayerData = leaderboardData.topPVPPlayers.Get(uiPvpIdx);
						if (uiPvpPlayerData)
						{
							TrackingModUI.LogRPC(string.Format("  [%1] PlayerName[%2] PVPPoints[%3] IsOnline[%4] PlayerID[%5]", 
								uiPvpIdx + 1, uiPvpPlayerData.playerName, uiPvpPlayerData.pvpPoints, uiPvpPlayerData.isOnline, uiPvpPlayerData.playerID));
						}
					}
				}
				
				if (leaderboardData.categoryPreviews && leaderboardData.categoryPreviews.Count() > 0)
				{
					TrackingModUI.LogRPC("Category Previews:");
					for (uiPreviewIdx = 0; uiPreviewIdx < leaderboardData.categoryPreviews.Count(); uiPreviewIdx++)
					{
						uiPreviewCatID = leaderboardData.categoryPreviews.GetKey(uiPreviewIdx);
						uiPreviewClass = leaderboardData.categoryPreviews.GetElement(uiPreviewIdx);
						TrackingModUI.LogRPC(string.Format("  CategoryID[%1] PreviewClass[%2]", uiPreviewCatID, uiPreviewClass));
					}
				}
				
				TrackingModUI.LogRPC("========================================");
				
				if (m_PendingPVEMode || (m_LeaderboardMenu && m_LeaderboardMenu.IsVisible()))
				{
					if (leaderboardData.disablePVELeaderboard)
					{
						if (player)
							player.MessageAction("[TrackingMod] PVE Leaderboard is disabled by server config");
						m_PendingPVEMode = false;
						return;
					}
					
				if (!m_LeaderboardMenu)
				{
					m_LeaderboardMenu = new TrackingModLeaderboardMenu();
					GetGame().GetMission().GetHud().Show(false);
				}
				
				if (m_LeaderboardMenu)
				{
					if (m_PendingPVEMode)
					{
						m_LeaderboardMenu.ClearCache();
					}
					m_LeaderboardMenu.SetLeaderboardData(leaderboardData);
				}
					
					m_PendingPVEMode = false;
				}
				else if (m_PendingPVPMode || (m_PvPLeaderboardMenu && m_PvPLeaderboardMenu.IsVisible()))
				{
					if (leaderboardData.disablePVPLeaderboard)
					{
						if (player)
							player.MessageAction("[TrackingMod] PVP Leaderboard is disabled by server config");
						m_PendingPVPMode = false;
						return;
					}
					
				if (!m_PvPLeaderboardMenu)
				{
					m_PvPLeaderboardMenu = new TrackingModPvPLeaderboardMenu();
					GetGame().GetMission().GetHud().Show(false);
				}
				
				if (m_PvPLeaderboardMenu)
					m_PvPLeaderboardMenu.SetLeaderboardData(leaderboardData);
					
					m_PendingPVPMode = false;
				}
			}
		}
	}
	
	void ReceiveTrackingModRewardClaim(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<TrackingModRewardClaimResponse> responseParam;
		TrackingModRewardClaimResponse response;
		PlayerBase player;
		TrackingModLeaderboardMenu pveMenu;
		TrackingModPvPLeaderboardMenu pvpMenu;
		
		if (type != CallType.Client)
			return;
		
		if (ctx.Read(responseParam))
		{
			response = responseParam.param1;
			player = PlayerBase.Cast(GetGame().GetPlayer());
			if (response && player)
			{
				if (response.success)
				{
					TrackingModUI.LogRPC("[ReceiveTrackingModRewardClaim] Reward claimed successfully. Remaining: " + response.remainingRewards.ToString());
				}
				else
				{
					TrackingModUI.LogRPC("[ReceiveTrackingModRewardClaim] Reward claim failed: " + response.message);
				}
				
				if (m_LeaderboardMenu && m_LeaderboardMenu.IsVisible())
				{
					m_LeaderboardMenu.UpdateRewardClaimResponse(response);
				}
				
				if (m_PvPLeaderboardMenu && m_PvPLeaderboardMenu.IsVisible())
				{
					m_PvPLeaderboardMenu.UpdateRewardClaimResponse(response);
				}
			}
		}
	}
	
	void ReceivePlayerDataUpdate(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, ref PlayerDeathData> dataParam;
		TrackingModData data;
		string playerID;
		PlayerDeathData playerData;
		
		if (type != CallType.Client)
			return;
		
		if (ctx.Read(dataParam))
		{
			playerID = dataParam.param1;
			playerData = dataParam.param2;
			
			if (playerID != "" && playerData)
			{
				data = TrackingModData.GetInstance();
				if (data)
				{
					data.SyncPlayerData(playerID, playerData);
					TrackingModUI.LogRPC("Received player data update: " + playerID + " (Local map now has " + data.m_PlayerDataMap.Count().ToString() + " players)");
				}
			}
		}
	}
	
	TrackingModLeaderboardData BuildLeaderboardDataFromMap(TrackingModData data, int requestedPage, bool isPVE)
	{
		TrackingModLeaderboardData leaderboardData;
		array<ref PlayerDeathData> sortedList;
		int pageSize;
		int totalPlayers;
		int totalPages;
		int startIdx;
		int endIdx;
		int displayIdx;
		PlayerDeathData displayPlayerData;
		TrackingModLeaderboardPlayerData leaderboardPlayer;
		PVECategoryConfig categoryConfig;
		array<ref PVECategory> categories;
		int catIdx;
		PVECategory category;
		int playerOnlineCounter;
		int checkIdx;
		PlayerDeathData checkPlayerData;
		
		if (!data || !data.m_PlayerDataMap)
			return null;
		
		pageSize = 25;
		
		if (isPVE)
		{
			sortedList = data.GetSortedPVEList();
		}
		else
		{
			sortedList = data.GetSortedPVPList();
		}
		
		if (!sortedList)
			return null;
		
		playerOnlineCounter = 0;
		for (checkIdx = 0; checkIdx < data.m_PlayerDataMap.Count(); checkIdx++)
		{
			checkPlayerData = data.m_PlayerDataMap.GetElement(checkIdx);
			if (checkPlayerData && checkPlayerData.playerIsOnline == 1)
			{
				playerOnlineCounter++;
			}
		}
		
		leaderboardData = new TrackingModLeaderboardData();
		leaderboardData.playerOnlineCounter = playerOnlineCounter;
		
		if (g_TrackingModConfig)
		{
			leaderboardData.showPlayerOnlineStatusPVE = g_TrackingModConfig.ShowPlayerOnlineStatusPVE;
			leaderboardData.showPlayerOnlineStatusPVP = g_TrackingModConfig.ShowPlayerOnlineStatusPVP;
			leaderboardData.disablePVPLeaderboard = g_TrackingModConfig.DisablePVPLeaderboard;
			leaderboardData.disablePVELeaderboard = g_TrackingModConfig.DisablePVELeaderboard;
			leaderboardData.survivorIconPathMale = g_TrackingModConfig.SurvivorIconPathMale;
			leaderboardData.survivorIconPathFemale = g_TrackingModConfig.SurvivorIconPathFemale;
		}
		
		categoryConfig = PVECategoryConfig.GetInstance();
		if (categoryConfig)
		{
			categories = categoryConfig.GetCategories();
			for (catIdx = 0; catIdx < categories.Count(); catIdx++)
			{
				category = categories.Get(catIdx);
				if (category && category.CategoryID != "")
				{
					if (category.ClassNamePreview != "")
					{
						leaderboardData.categoryPreviews.Set(category.CategoryID, category.ClassNamePreview);
					}
				}
			}
		}
		
		totalPlayers = sortedList.Count();
		totalPages = (totalPlayers + pageSize - 1) / pageSize;
		if (totalPages < 1)
			totalPages = 1;
		
		if (requestedPage > totalPages)
			requestedPage = totalPages;
		
		startIdx = (requestedPage - 1) * pageSize;
		endIdx = startIdx + pageSize;
		if (endIdx > totalPlayers)
			endIdx = totalPlayers;
		
		leaderboardData.currentPage = requestedPage;
		leaderboardData.totalPages = totalPages;
		leaderboardData.totalPlayers = totalPlayers;
		
		if (g_TrackingModConfig)
		{
			if (isPVE)
				leaderboardData.maxDisplayCount = g_TrackingModConfig.MaxPVEPlayersDisplay;
			else
				leaderboardData.maxDisplayCount = g_TrackingModConfig.MaxPVPPlayersDisplay;
		}
		else
		{
			if (isPVE)
				leaderboardData.maxDisplayCount = 100;
			else
				leaderboardData.maxDisplayCount = 24;
		}
		
		leaderboardData.playersOnCurrentPage = endIdx - startIdx;
		
		for (displayIdx = startIdx; displayIdx < endIdx; displayIdx++)
		{
			displayPlayerData = sortedList.Get(displayIdx);
			if (displayPlayerData)
			{
				leaderboardPlayer = CreateLeaderboardPlayerData(displayPlayerData);
				if (isPVE)
				{
					leaderboardData.topPVEPlayers.Insert(leaderboardPlayer);
				}
				else
				{
					leaderboardData.topPVPPlayers.Insert(leaderboardPlayer);
				}
			}
		}
		
		return leaderboardData;
	}
	
	TrackingModLeaderboardPlayerData CreateLeaderboardPlayerData(PlayerDeathData playerData)
	{
		TrackingModLeaderboardPlayerData leaderboardPlayer;
		int killIdx;
		string killCategoryID;
		int kills;
		int deathIdx;
		string deathCategoryID;
		int deaths;
		int rangeIdx;
		string rangeCategoryID;
		int longestRange;
		
		if (!playerData)
			return null;
		
		leaderboardPlayer = new TrackingModLeaderboardPlayerData();
		leaderboardPlayer.playerID = playerData.PlayerID;
		leaderboardPlayer.playerName = playerData.PlayerName;
		leaderboardPlayer.deathCount = playerData.GetTotalPVPDeaths();
		leaderboardPlayer.pvePoints = playerData.GetPVEPoints();
		leaderboardPlayer.pvpPoints = playerData.GetPVPPoints();
		leaderboardPlayer.survivorType = playerData.survivorType;
		leaderboardPlayer.isOnline = playerData.playerIsOnline;
		
		if (g_TrackingModRewardConfig && g_TrackingModRewardConfig.EnableMilestoneRewards)
		{
			leaderboardPlayer.pendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
		}
		else
		{
			leaderboardPlayer.pendingRewards = 0;
		}
		
		if (playerData.CategoryKills)
		{
			leaderboardPlayer.categoryKills = new map<string, int>();
			for (killIdx = 0; killIdx < playerData.CategoryKills.Count(); killIdx++)
			{
				killCategoryID = playerData.CategoryKills.GetKey(killIdx);
				kills = playerData.CategoryKills.GetElement(killIdx);
				leaderboardPlayer.categoryKills.Set(killCategoryID, kills);
			}
		}
		
		if (playerData.CategoryDeaths)
		{
			leaderboardPlayer.categoryDeaths = new map<string, int>();
			for (deathIdx = 0; deathIdx < playerData.CategoryDeaths.Count(); deathIdx++)
			{
				deathCategoryID = playerData.CategoryDeaths.GetKey(deathIdx);
				deaths = playerData.CategoryDeaths.GetElement(deathIdx);
				leaderboardPlayer.categoryDeaths.Set(deathCategoryID, deaths);
			}
		}
		
		if (playerData.CategoryLongestRanges)
		{
			leaderboardPlayer.categoryLongestRanges = new map<string, int>();
			for (rangeIdx = 0; rangeIdx < playerData.CategoryLongestRanges.Count(); rangeIdx++)
			{
				rangeCategoryID = playerData.CategoryLongestRanges.GetKey(rangeIdx);
				longestRange = playerData.CategoryLongestRanges.GetElement(rangeIdx);
				leaderboardPlayer.categoryLongestRanges.Set(rangeCategoryID, longestRange);
			}
		}
		
		return leaderboardPlayer;
	}
}

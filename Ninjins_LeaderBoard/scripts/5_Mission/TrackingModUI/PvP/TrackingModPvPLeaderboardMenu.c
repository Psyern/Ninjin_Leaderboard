class TrackingModPvPLeaderboardMenu: ScriptViewMenu
{
	protected ref TrackingModPvPLeaderboardController m_MenuController;
	protected ref TrackingModLeaderboardData m_LeaderboardData;
	protected int m_CurrentPage;
	protected bool m_IsPVPMode;
	protected ref map<int, ref TrackingModLeaderboardData> m_PageCache;
	protected int m_NextPreloadPage;
	protected int m_MaxPreloadPage;
	protected ButtonWidget m_SettingsButton;
	
	void TrackingModPvPLeaderboardMenu()
	{
		m_MenuController = TrackingModPvPLeaderboardController.Cast(m_Controller);
		m_CurrentPage = 1;
		m_IsPVPMode = true;
		m_PageCache = new map<int, ref TrackingModLeaderboardData>();
		m_NextPreloadPage = 0;
		m_MaxPreloadPage = 0;
	}
	
	void ~TrackingModPvPLeaderboardMenu()
	{
		ClearCache();
		UnlockControls();
	}
	
	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		InitializeWidgets();
		LockControls();
		SetFocus(GetLayoutRoot());
	}

	protected void InitializeWidgets()
	{
		if (!GetLayoutRoot())
			return;

		m_SettingsButton = ButtonWidget.Cast(GetLayoutRoot().FindAnyWidget("settings_button"));
		UpdateSettingsButtonVisibility();
	}
	
	void SetLeaderboardData(TrackingModLeaderboardData data)
	{
		int pageNumber;
		
		if (!data)
			return;
		
		pageNumber = data.currentPage;
		if (pageNumber > 0)
		{
			m_PageCache.Set(pageNumber, data);
			TrackingModUI.LogRPC(string.Format("[Cache] Stored page %1 in cache (Total cached: %2 pages)", pageNumber, m_PageCache.Count()));
		}
		
		if (!m_LeaderboardData || m_LeaderboardData.currentPage == pageNumber)
		{
			m_LeaderboardData = data;
			LoadPlayers();
			UpdateSettingsButtonVisibility();
			
			if (pageNumber == 1 && data.totalPages > 1)
			{
				StartSequentialPreload(data.totalPages);
			}
		}
		else
		{
			TrackingModUI.LogRPC(string.Format("[Cache] Page %1 received but not displayed (currently viewing page %2)", pageNumber, m_LeaderboardData.currentPage));
			
			if (m_NextPreloadPage > 0 && pageNumber == m_NextPreloadPage)
			{
				ContinueSequentialPreload();
			}
		}
	}
	
	void StartSequentialPreload(int totalPages)
	{
		if (totalPages <= 1)
			return;
		
		m_MaxPreloadPage = Math.Min(4, totalPages);
		m_NextPreloadPage = 2;
		
		TrackingModUI.LogRPC(string.Format("[Cache] Starting sequential preload of pages 2-%1", m_MaxPreloadPage));
		ContinueSequentialPreload();
	}
	
	void ContinueSequentialPreload()
	{
		PlayerBase player;
		
		if (m_NextPreloadPage <= 0 || m_NextPreloadPage > m_MaxPreloadPage)
		{
			if (m_NextPreloadPage > m_MaxPreloadPage)
			{
				TrackingModUI.LogRPC(string.Format("[Cache] Sequential preload complete (all %1 pages loaded)", m_MaxPreloadPage));
				m_NextPreloadPage = 0;
			}
			return;
		}
		
		if (m_PageCache.Contains(m_NextPreloadPage))
		{
			TrackingModUI.LogRPC(string.Format("[Cache] Page %1 already cached, skipping", m_NextPreloadPage));
			m_NextPreloadPage++;
			ContinueSequentialPreload();
			return;
		}
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;
		
		TrackingModUI.LogRPC(string.Format("[RPC] Preloading page %1/%2 from server", m_NextPreloadPage, m_MaxPreloadPage));
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "RequestTrackingModLeaderboard", new Param1<int>(m_NextPreloadPage), true, player.GetIdentity());
		m_NextPreloadPage++;
	}
	
	void ClearCache()
	{
		int cachedCount;
		
		if (m_PageCache)
		{
			cachedCount = m_PageCache.Count();
			m_PageCache.Clear();
			TrackingModUI.LogRPC(string.Format("[Cache] Cleared cache (was storing %1 pages)", cachedCount));
		}
		m_NextPreloadPage = 0;
		m_MaxPreloadPage = 0;
	}
	
	TrackingModLeaderboardData GetCachedPage(int page)
	{
		if (!m_PageCache || page <= 0)
			return null;
		
		if (m_PageCache.Contains(page))
		{
			TrackingModUI.LogRPC(string.Format("[Cache] Using cached page %1 (Total cached: %2 pages)", page, m_PageCache.Count()));
			return m_PageCache.Get(page);
		}
		
		return null;
	}
	
	void LoadPlayers()
	{
		if (!m_LeaderboardData)
			return;
		
		m_MenuController.PlayerEntries.Clear();
		
		m_MenuController.PlayerOnlineCounter = string.Format("Online: %1", m_LeaderboardData.playerOnlineCounter);
		
		if (m_IsPVPMode && m_LeaderboardData.topPVPPlayers)
		{
			m_CurrentPage = m_LeaderboardData.currentPage;
			
			if (m_CurrentPage > m_LeaderboardData.totalPages)
			{
				TrackingModUI.LogRPC(string.Format("[Cache] Warning: Current page %1 exceeds total pages %2, clamping to last page", m_CurrentPage, m_LeaderboardData.totalPages));
				m_CurrentPage = m_LeaderboardData.totalPages;
			}
			
			int playersOnPage;
			playersOnPage = m_LeaderboardData.playersOnCurrentPage;
			if (playersOnPage > 0)
				m_MenuController.TopX = string.Format("Page %1/%2 - %3 out of %4", m_CurrentPage, m_LeaderboardData.totalPages, playersOnPage, m_LeaderboardData.totalPlayers);
			else
				m_MenuController.TopX = string.Format("Page %1/%2 (%3 players)", m_CurrentPage, m_LeaderboardData.totalPages, m_LeaderboardData.totalPlayers);
			m_MenuController.NotifyPropertiesChanged({"PlayerOnlineCounter", "TopX"});
			
			if (m_LeaderboardData.topPVPPlayers.Count() == 0)
			{
				TrackingModUI.LogRPC(string.Format("[Cache] Warning: Page %1 has no players", m_CurrentPage));
				return;
			}
			
			int i;
			TrackingModLeaderboardPlayerData playerData;
			int position;
			TrackingModPvPPlayerEntry entry;
			
			for (i = 0; i < m_LeaderboardData.topPVPPlayers.Count(); i++)
			{
				playerData = m_LeaderboardData.topPVPPlayers.Get(i);
				if (playerData)
				{
					position = ((m_CurrentPage - 1) * 25) + i + 1;
					entry = new TrackingModPvPPlayerEntry(playerData, position);
					entry.SetShowOnlineStatus(m_LeaderboardData.showPlayerOnlineStatusPVP);
					entry.SetSurvivorIconPaths(m_LeaderboardData.survivorIconPathMale, m_LeaderboardData.survivorIconPathFemale);
					entry.LoadIconImage();
					entry.UpdateClaimRewardButtonVisibility();
					m_MenuController.PlayerEntries.Insert(entry);
				}
			}
		}
	}
	
	void RequestPage(int page)
	{
		int maxPage;
		TrackingModLeaderboardData cachedData;
		PlayerBase player;
		
		if (page <= 0)
			return;
		
		maxPage = 4;
		if (m_LeaderboardData)
		{
			maxPage = Math.Min(4, m_LeaderboardData.totalPages);
			if (page > maxPage)
			{
				TrackingModUI.LogRPC(string.Format("[Cache] Page %1 exceeds max pages (%2), ignoring request", page, maxPage));
				return;
			}
		}
		else if (page > maxPage)
		{
			TrackingModUI.LogRPC(string.Format("[Cache] Page %1 exceeds max pages (%2), ignoring request", page, maxPage));
			return;
		}
		
		m_CurrentPage = page;
		
		cachedData = GetCachedPage(page);
		if (cachedData)
		{
			m_LeaderboardData = cachedData;
			LoadPlayers();
			return;
		}
		
		TrackingModUI.LogRPC(string.Format("[RPC] Page %1 not in cache, requesting from server", page));
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;
		
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "RequestTrackingModLeaderboard", new Param1<int>(page), true, player.GetIdentity());
	}
	
	void OnClickPageFirst()
	{
		RequestPage(1);
	}
	
	void OnClickPagePrev()
	{
		if (m_CurrentPage > 1)
			RequestPage(m_CurrentPage - 1);
	}
	
	void OnClickPageNext()
	{
		int maxPage;
		
		if (!m_LeaderboardData)
			return;
		
		maxPage = Math.Min(4, m_LeaderboardData.totalPages);
		if (m_CurrentPage >= maxPage)
			return;
		
		RequestPage(m_CurrentPage + 1);
	}
	
	void OnClickPageLast()
	{
		int maxPage;
		
		if (!m_LeaderboardData)
			return;
		
		maxPage = Math.Min(4, m_LeaderboardData.totalPages);
		if (maxPage > 0)
			RequestPage(maxPage);
	}

	void UpdateSettingsButtonVisibility()
	{
		if (!m_SettingsButton)
			return;

		if (!m_LeaderboardData)
		{
			m_SettingsButton.Show(false);
			return;
		}

		m_SettingsButton.Show(m_LeaderboardData.isAdmin);
	}

	void OnClickSettings()
	{
		TrackingModAdminMenu adminMenu;

		if (!m_LeaderboardData || !m_LeaderboardData.isAdmin)
			return;

		adminMenu = new TrackingModAdminMenu();
		if (adminMenu)
			adminMenu.SetLeaderboardData(m_LeaderboardData);

		ClearCache();
		Close();
	}
	
	void LockControls()
	{
		ForceDisableInputs(true);
	}
	
	void UnlockControls()
	{
		ForceDisableInputs(false);
	}
	
	void ForceDisableInputs(bool state)
	{
		TIntArray skip;
		TIntArray inputIDs;
		int i;
		int inputID;
		UAInput input;
		
		skip = new TIntArray;
		skip.Insert(UAUIBack);
		skip.Insert(UAOpenTrackingModLeaderboardPVP);
		
		inputIDs = new TIntArray;
		GetUApi().GetActiveInputs(inputIDs);
		
		for (i = 0; i < inputIDs.Count(); i++)
		{
			inputID = inputIDs.Get(i);
			if (skip.Find(inputID) == -1)
			{
				input = GetUApi().GetInputByID(inputID);
				if (input)
					input.ForceDisable(state);
			}
		}
	}
	
	override array<string> GetInputExcludes()
	{
		return {"menu"};
	}
	
	override bool UseMouse()
	{
		return true;
	}
	
	override bool CanCloseWithEscape()
	{
		return true;
	}
	
	override void Update(float dt)
	{
		super.Update(dt);
		
		UAInput input;
		input = GetUApi().GetInputByName("UAUIBack");
		if (input && input.LocalPress())
		{
			TrackingModUI.LogRPC("[Cache] ESC pressed, clearing cache before close");
			ClearCache();
			GetGame().GetMission().GetHud().Show(true);
			Close();
		}
	}
	
	override typename GetControllerType()
	{
		return TrackingModPvPLeaderboardController;
	}
	

	void UpdateRewardClaimResponse(TrackingModRewardClaimResponse response)
	{
		int i;
		TrackingModPvPPlayerEntry entry;
		TrackingModLeaderboardPlayerData playerData;
		PlayerBase player;
		PlayerIdentity identity;
		string currentPlayerID;
		
		if (!response || !m_MenuController || !m_MenuController.PlayerEntries)
			return;
		
		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		
		identity = player.GetIdentity();
		if (!identity)
			return;
		
		currentPlayerID = identity.GetPlainId();
		
		for (i = 0; i < m_MenuController.PlayerEntries.Count(); i++)
		{
			entry = m_MenuController.PlayerEntries.Get(i);
			if (entry)
			{
				playerData = entry.GetPlayerData();
				if (playerData && playerData.playerID == currentPlayerID)
				{
					if (response.success)
					{
						playerData.pendingRewards = response.remainingRewards;
						entry.UpdateClaimRewardButtonVisibility();
					}
					break;
				}
			}
		}
	}

	override string GetLayoutFile()
	{
		return "Ninjins_LeaderBoard\\GUI\\layouts\\PvP\\trackingModPvPLeaderboard_mvc.layout";
	}
	
}

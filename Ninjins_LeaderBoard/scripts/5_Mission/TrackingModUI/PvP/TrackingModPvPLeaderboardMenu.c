class TrackingModPvPLeaderboardMenu: ScriptViewMenu
{
	protected static ref TrackingModPvPLeaderboardMenu s_NJN_Instance;
	protected ref TrackingModPvPLeaderboardController m_MenuController;
	protected ref TrackingModLeaderboardData m_LeaderboardData;
	protected int m_CurrentPage;
	protected bool m_IsPVPMode;
	protected ref map<int, ref TrackingModLeaderboardData> m_PageCache;
	protected int m_NextPreloadPage;
	protected int m_MaxPreloadPage;
	protected ButtonWidget m_SettingsButton;
	protected ref TrackingModLeaderboardPlayerData m_NJN_OwnPlayerStats;

	void TrackingModPvPLeaderboardMenu()
	{
		s_NJN_Instance = this;
		m_MenuController = TrackingModPvPLeaderboardController.Cast(m_Controller);
		m_CurrentPage = 1;
		m_IsPVPMode = true;
		m_PageCache = new map<int, ref TrackingModLeaderboardData>();
		m_NextPreloadPage = 0;
		m_MaxPreloadPage = 0;
		TrackingModColumnSort.ClearSort();
	}

	void ~TrackingModPvPLeaderboardMenu()
	{
		if (s_NJN_Instance == this)
			s_NJN_Instance = null;
		ClearCache();
		UnlockControls();
	}

	static TrackingModPvPLeaderboardMenu GetInstance()
	{
		return s_NJN_Instance;
	}

	void SetOwnPlayerStats(TrackingModLeaderboardPlayerData ownStats)
	{
		m_NJN_OwnPlayerStats = ownStats;
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
		UpdateSortIndicators();
	}

	// Column ID (see TrackingModColumnSort) for a clickable header widget.
	// Empty string means the widget is not a sortable column.
	protected string GetColumnIDForHeader(string widgetName)
	{
		if (widgetName == "th_player")
			return "Name";
		if (widgetName == "th_points")
			return "PvPPoints";
		if (widgetName == "th_range")
			return "PvPLongestRange";
		if (widgetName == "th_kills")
			return "PlayerKills";
		if (widgetName == "th_deaths")
			return "Deaths";
		if (widgetName == "th_ratio")
			return "KDRatio";
		if (widgetName == "th_playtime")
			return "PlayTime";
		if (widgetName == "th_reputation")
			return "Reputation";
		return "";
	}

	// Base captions as set in trackingModPvPLeaderboard_mvc.layout. TextWidget
	// has no GetText(), so the labels are mirrored here for the sort indicator.
	protected string GetHeaderBaseLabel(string widgetName)
	{
		if (widgetName == "th_player")
			return "Player";
		if (widgetName == "th_points")
			return "Points";
		if (widgetName == "th_range")
			return "Range";
		if (widgetName == "th_kills")
			return "Kills";
		if (widgetName == "th_deaths")
			return "Deaths";
		if (widgetName == "th_ratio")
			return "K/D";
		if (widgetName == "th_playtime")
			return "Spielzeit";
		if (widgetName == "th_reputation")
			return "Ruf";
		return "";
	}

	protected TStringArray GetSortableHeaderNames()
	{
		return {"th_player", "th_points", "th_range", "th_kills", "th_deaths", "th_ratio", "th_playtime", "th_reputation"};
	}

	protected void UpdateSortIndicators()
	{
		TStringArray headerNames;
		string widgetName;
		string columnID;
		string label;
		string currentColumn;
		TextWidget headerWidget;
		int i;

		if (!GetLayoutRoot())
			return;

		headerNames = GetSortableHeaderNames();
		currentColumn = TrackingModColumnSort.GetCurrentColumn();

		for (i = 0; i < headerNames.Count(); i++)
		{
			widgetName = headerNames.Get(i);
			headerWidget = TextWidget.Cast(GetLayoutRoot().FindAnyWidget(widgetName));
			if (!headerWidget)
				continue;

			label = GetHeaderBaseLabel(widgetName);
			columnID = GetColumnIDForHeader(widgetName);
			if (columnID != "" && columnID == currentColumn)
			{
				if (TrackingModColumnSort.IsAscending())
					label = label + " ^";
				else
					label = label + " v";
			}
			headerWidget.SetText(label);
		}
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		bool handled;
		string widgetName;
		string columnID;

		handled = super.OnMouseButtonDown(w, x, y, button);

		if (!w || button != MouseState.LEFT)
			return handled;

		widgetName = w.GetName();

		// Clicking the rank header restores the server-side points order.
		if (widgetName == "th_rank")
		{
			if (TrackingModColumnSort.GetCurrentColumn() == "")
				return handled;
			TrackingModColumnSort.ClearSort();
			LoadPlayers();
			UpdateSortIndicators();
			return true;
		}

		columnID = GetColumnIDForHeader(widgetName);
		if (columnID == "")
			return handled;

		TrackingModColumnSort.SetSort(columnID);
		LoadPlayers();
		UpdateSortIndicators();
		return true;
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
		
		player = PlayerBase.Cast(g_Game.GetPlayer());
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

		Widget reputationHeader;
		if (GetLayoutRoot())
		{
			reputationHeader = GetLayoutRoot().FindAnyWidget("th_reputation");
			if (reputationHeader)
				reputationHeader.Show(m_LeaderboardData.showReputation);
		}

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
			int originalIndex;
			TrackingModPvPPlayerEntry entry;
			array<ref TrackingModLeaderboardPlayerData> displayPlayers;

			// Render from a copy so the cached page keeps the server-side
			// points order; the active column sort only affects the display.
			displayPlayers = new array<ref TrackingModLeaderboardPlayerData>();
			for (i = 0; i < m_LeaderboardData.topPVPPlayers.Count(); i++)
			{
				displayPlayers.Insert(m_LeaderboardData.topPVPPlayers.Get(i));
			}

			if (TrackingModColumnSort.GetCurrentColumn() != "")
				TrackingModColumnSort.SortPlayerList(displayPlayers);

			for (i = 0; i < displayPlayers.Count(); i++)
			{
				playerData = displayPlayers.Get(i);
				if (playerData)
				{
					// Rank stays the points-based position from the server list.
					originalIndex = m_LeaderboardData.topPVPPlayers.Find(playerData);
					if (originalIndex == -1)
						originalIndex = i;
					position = ((m_CurrentPage - 1) * 25) + originalIndex + 1;
					entry = new TrackingModPvPPlayerEntry(playerData, position);
					entry.SetShowOnlineStatus(m_LeaderboardData.showPlayerOnlineStatusPVP);
					entry.SetShowReputation(m_LeaderboardData.showReputation);
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
		player = PlayerBase.Cast(g_Game.GetPlayer());
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
			g_Game.GetMission().GetHud().Show(true);
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
		
		player = PlayerBase.Cast(g_Game.GetPlayer());
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

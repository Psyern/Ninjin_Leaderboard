modded class MissionServer extends MissionBase
{
	protected ref map<string, int> m_LeaderboardRequestTimes;
	protected const int LEADERBOARD_RATE_LIMIT_MS = 500;
	protected float m_NJN_DistanceTrackTimer;
	#ifdef Psyerns_Framework
	protected float m_WebExportTimer;
	protected float m_WebExportInterval;
	protected bool m_WebExportEnabled;
	#endif
	
	override void OnInit()
	{
		TrackingModData data;
		#ifdef NinjinsPvPPvE
		TrackingModZoneHandler stateHandler;
		TrackingModZoneEventCallback zoneCallback;
		#endif
		
		super.OnInit();
		m_LeaderboardRequestTimes = new map<string, int>();
		m_NJN_DistanceTrackTimer = 0;
		if (GetGame().IsDedicatedServer())
		{
			TrackingModData.CheckDirectories();
			data = TrackingModData.LoadData();
			PVECategoryConfig.GetInstance();
			PVPCategoryConfig.GetInstance();
			DeathCategoryConfig.GetInstance();
			g_TrackingModConfig = TrackingModConfig.LoadConfig();
			g_TrackingModRewardConfig = TrackingModRewardConfig.LoadConfig();
			TrackingModPermissions.GetInstance().LoadPermissions();
			g_TrackingModShopConfig = TrackingModShopConfig.LoadConfig();
			g_TrackingModStyleConfig = TrackingModStyleConfig.LoadConfig();

			TrackingModData.CleanupOldFiles();
			
			if (data)
			{
				data.MarkLeaderboardDirty();
				data.RebuildSortedLists();
				data.ExportWebLeaderboard();
			}
			
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "RequestTrackingModLeaderboard", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "ClaimTrackingModReward", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "RequestAdminConfig", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "SaveAdminConfig", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "SaveAdminFullConfig", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "AdminSearchPlayer", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "AdminWipePlayerData", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "AdminWipePlayerPoints", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "AdminAwardPoints", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "AdminAwardItem", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "RequestShopConfig", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "PurchaseShopItem", this, SingleplayerExecutionType.Server);
			GetRPCManager().AddRPC("Ninjins_LeaderBoard", "RequestStyleConfig", this, SingleplayerExecutionType.Server);

			#ifdef NinjinsPvPPvE
			stateHandler = new TrackingModZoneHandler();
			PlayerZoneController.RegisterPlayerStateChangeCallback(stateHandler);
			zoneCallback = new TrackingModZoneEventCallback();
			PlayerZoneController.RegisterZoneEventCallback(zoneCallback);
			TrackingMod.LogInfo("Zone callbacks registered with NinjinsPvPPvE");
			#endif
			TrackingMod.LogInfo("Server initialized - Directories checked/created, data loaded, categories initialized, config loaded");

			#ifdef Psyerns_Framework
			m_WebExportEnabled = false;
			m_WebExportTimer = 0;
			m_WebExportInterval = 300;
			if (g_TrackingModConfig && g_TrackingModConfig.WebExportIntervalSeconds > 0)
				m_WebExportInterval = g_TrackingModConfig.WebExportIntervalSeconds;
			if (PF_WebConfig.GetInstance().IsEndpointEnabled("Leaderboard"))
			{
				TrackingModWebExportHelper.Init();
				m_WebExportEnabled = TrackingModWebExportHelper.IsInitialized();
				if (m_WebExportEnabled)
					TrackingMod.LogInfo("Web export enabled - interval: " + m_WebExportInterval.ToString() + "s");
			}
			#endif
		}
	}
	
	override void OnUpdate(float timeslice)
	{
		array<Man> allPlayers;
		int pIdx;
		PlayerBase pb;

		super.OnUpdate(timeslice);

		if (g_Game && g_Game.IsDedicatedServer())
		{
			m_NJN_DistanceTrackTimer = m_NJN_DistanceTrackTimer + timeslice;
			if (m_NJN_DistanceTrackTimer >= 30.0)
			{
				m_NJN_DistanceTrackTimer = 0;
				allPlayers = new array<Man>();
				g_Game.GetPlayers(allPlayers);
				for (pIdx = 0; pIdx < allPlayers.Count(); pIdx++)
				{
					pb = PlayerBase.Cast(allPlayers.Get(pIdx));
					if (pb)
					{
						pb.NJN_UpdateDistanceTracking();
					}
				}
			}
		}

		#ifdef Psyerns_Framework
		if (m_WebExportEnabled && GetGame().IsDedicatedServer())
		{
			m_WebExportTimer += timeslice;
			if (m_WebExportTimer >= m_WebExportInterval)
			{
				m_WebExportTimer = 0;
				TrackingModWarHardlineSync.SyncAllPlayers();
				TrackingModWebExportHelper.SendExport();
			}
		}
		#endif
	}

	protected bool IsSenderTrackingModAdmin(PlayerIdentity sender)
	{
		if (!sender || !g_TrackingModConfig)
			return false;

		return g_TrackingModConfig.IsAdmin(sender.GetPlainId());
	}

	protected PVECategory ClonePVECategory(PVECategory sourceCategory)
	{
		PVECategory clonedCategory;
		int i;

		if (!sourceCategory)
			return null;

		clonedCategory = new PVECategory();
		clonedCategory.CategoryID = sourceCategory.CategoryID;
		clonedCategory.ClassNamePreview = sourceCategory.ClassNamePreview;
		for (i = 0; i < sourceCategory.ClassNames.Count(); i++)
			clonedCategory.ClassNames.Insert(sourceCategory.ClassNames.Get(i));

		return clonedCategory;
	}

	protected PVPCategory ClonePVPCategory(PVPCategory sourceCategory)
	{
		PVPCategory clonedCategory;
		int i;

		if (!sourceCategory)
			return null;

		clonedCategory = new PVPCategory();
		clonedCategory.CategoryID = sourceCategory.CategoryID;
		clonedCategory.DisplayName = sourceCategory.DisplayName;
		for (i = 0; i < sourceCategory.ClassNames.Count(); i++)
			clonedCategory.ClassNames.Insert(sourceCategory.ClassNames.Get(i));

		return clonedCategory;
	}

	protected TrackingModGeneralAdminData BuildGeneralAdminData()
	{
		TrackingModGeneralAdminData data;
		int i;

		data = new TrackingModGeneralAdminData();
		if (!g_TrackingModConfig)
			return data;

		if (g_TrackingModConfig.AdminIDs)
		{
			for (i = 0; i < g_TrackingModConfig.AdminIDs.Count(); i++)
			{
				data.AdminIDs.Insert(g_TrackingModConfig.AdminIDs.Get(i));
			}
		}

		data.DeletePlayerFilesOlderThanDays = g_TrackingModConfig.DeletePlayerFilesOlderThanDays;
		data.UseUTCForDates = g_TrackingModConfig.UseUTCForDates;
		data.EnableRewardSystem = g_TrackingModConfig.EnableRewardSystem;
		data.DisablePVPLeaderboard = g_TrackingModConfig.DisablePVPLeaderboard;
		data.DisablePVELeaderboard = g_TrackingModConfig.DisablePVELeaderboard;
		data.MaxPVEPlayersDisplay = g_TrackingModConfig.MaxPVEPlayersDisplay;
		data.MaxPVPPlayersDisplay = g_TrackingModConfig.MaxPVPPlayersDisplay;
		data.ShowPlayerOnlineStatusPVE = g_TrackingModConfig.ShowPlayerOnlineStatusPVE;
		data.ShowPlayerOnlineStatusPVP = g_TrackingModConfig.ShowPlayerOnlineStatusPVP;
		data.DisableDeathBySuicide = g_TrackingModConfig.DisableDeathBySuicide;
		data.DisableDeathByGrenade = g_TrackingModConfig.DisableDeathByGrenade;
		data.DisableDeathByTrap = g_TrackingModConfig.DisableDeathByTrap;
		data.DisableDeathByZombie = g_TrackingModConfig.DisableDeathByZombie;
		data.DisableDeathByAnimal = g_TrackingModConfig.DisableDeathByAnimal;
		data.DisableDeathByAI = g_TrackingModConfig.DisableDeathByAI;
		data.DisableDeathByCar = g_TrackingModConfig.DisableDeathByCar;
		data.DisableDeathByWeapon = g_TrackingModConfig.DisableDeathByWeapon;
		data.DisableDeathByUnarmed = g_TrackingModConfig.DisableDeathByUnarmed;
		data.DisableDeathByUnknown = g_TrackingModConfig.DisableDeathByUnknown;
		data.DisableKillByGrenade = g_TrackingModConfig.DisableKillByGrenade;
		data.DisableKillByTrap = g_TrackingModConfig.DisableKillByTrap;
		data.DisableKillByAnimal = g_TrackingModConfig.DisableKillByAnimal;
		data.DisableKillByZombie = g_TrackingModConfig.DisableKillByZombie;
		data.DisableKillByCar = g_TrackingModConfig.DisableKillByCar;
		data.DisableKillByWeapon = g_TrackingModConfig.DisableKillByWeapon;
		data.DisableKillByUnarmed = g_TrackingModConfig.DisableKillByUnarmed;
		data.DisableKillByUnknown = g_TrackingModConfig.DisableKillByUnknown;
		data.DisableKillByUnconsciousSuicide = g_TrackingModConfig.DisableKillByUnconsciousSuicide;
		if (g_TrackingModConfig.TrackPVEKillsInZoneTypes)
		{
			for (i = 0; i < g_TrackingModConfig.TrackPVEKillsInZoneTypes.Count(); i++)
			{
				data.TrackPVEKillsInZoneTypes.Insert(g_TrackingModConfig.TrackPVEKillsInZoneTypes.Get(i));
			}
		}
		data.ExcludePVPKillWhenBothInPVE = g_TrackingModConfig.ExcludePVPKillWhenBothInPVE;
		data.EnableWebExport = g_TrackingModConfig.EnableWebExport;
		data.WebExportIntervalSeconds = g_TrackingModConfig.WebExportIntervalSeconds;
		data.WebExportMaxPlayers = g_TrackingModConfig.WebExportMaxPlayers;
		data.WebExportIncludePlayerIDs = g_TrackingModConfig.WebExportIncludePlayerIDs;

		return data;
	}

	protected TrackingModPVEAdminData BuildPVEAdminData()
	{
		TrackingModPVEAdminData data;
		PVECategoryConfig config;
		array<ref PVECategory> categories;
		PVECategory category;
		int i;

		data = new TrackingModPVEAdminData();
		config = PVECategoryConfig.GetInstance();
		if (!config)
			return data;

		data.PVEDeathPenaltyPoints = config.GetPVEDeathPenaltyPoints();
		categories = config.GetCategories();
		for (i = 0; i < categories.Count(); i++)
		{
			category = ClonePVECategory(categories.Get(i));
			if (category)
				data.Categories.Insert(category);
		}

		return data;
	}

	protected TrackingModPVPAdminData BuildPVPAdminData()
	{
		TrackingModPVPAdminData data;
		PVPCategoryConfig config;
		array<ref PVPCategory> categories;
		PVPCategory category;
		int i;

		data = new TrackingModPVPAdminData();
		config = PVPCategoryConfig.GetInstance();
		if (!config)
			return data;

		data.PVPDeathPenaltyPoints = config.GetPVPDeathPenaltyPoints();
		categories = config.GetCategories();
		for (i = 0; i < categories.Count(); i++)
		{
			category = ClonePVPCategory(categories.Get(i));
			if (category)
				data.Categories.Insert(category);
		}

		return data;
	}

	protected TrackingModAdminConfigData BuildAdminConfigData()
	{
		TrackingModAdminConfigData data;

		data = new TrackingModAdminConfigData();
		data.GeneralSettings = BuildGeneralAdminData();
		data.PVESettings = BuildPVEAdminData();
		data.PVPSettings = BuildPVPAdminData();

		return data;
	}

	protected void ApplyGeneralAdminData(TrackingModGeneralAdminData data)
	{
		int i;

		if (!g_TrackingModConfig || !data)
			return;

		g_TrackingModConfig.AdminIDs.Clear();
		if (data.AdminIDs)
		{
			for (i = 0; i < data.AdminIDs.Count(); i++)
			{
				if (data.AdminIDs.Get(i) != "")
					g_TrackingModConfig.AdminIDs.Insert(data.AdminIDs.Get(i));
			}
		}

		g_TrackingModConfig.DeletePlayerFilesOlderThanDays = Math.Max(0, data.DeletePlayerFilesOlderThanDays);
		g_TrackingModConfig.UseUTCForDates = data.UseUTCForDates;
		g_TrackingModConfig.EnableRewardSystem = data.EnableRewardSystem;
		g_TrackingModConfig.DisablePVPLeaderboard = data.DisablePVPLeaderboard;
		g_TrackingModConfig.DisablePVELeaderboard = data.DisablePVELeaderboard;
		g_TrackingModConfig.MaxPVEPlayersDisplay = Math.Max(1, Math.Min(data.MaxPVEPlayersDisplay, 100));
		g_TrackingModConfig.MaxPVPPlayersDisplay = Math.Max(1, Math.Min(data.MaxPVPPlayersDisplay, 100));
		g_TrackingModConfig.ShowPlayerOnlineStatusPVE = data.ShowPlayerOnlineStatusPVE;
		g_TrackingModConfig.ShowPlayerOnlineStatusPVP = data.ShowPlayerOnlineStatusPVP;
		g_TrackingModConfig.DisableDeathBySuicide = data.DisableDeathBySuicide;
		g_TrackingModConfig.DisableDeathByGrenade = data.DisableDeathByGrenade;
		g_TrackingModConfig.DisableDeathByTrap = data.DisableDeathByTrap;
		g_TrackingModConfig.DisableDeathByZombie = data.DisableDeathByZombie;
		g_TrackingModConfig.DisableDeathByAnimal = data.DisableDeathByAnimal;
		g_TrackingModConfig.DisableDeathByAI = data.DisableDeathByAI;
		g_TrackingModConfig.DisableDeathByCar = data.DisableDeathByCar;
		g_TrackingModConfig.DisableDeathByWeapon = data.DisableDeathByWeapon;
		g_TrackingModConfig.DisableDeathByUnarmed = data.DisableDeathByUnarmed;
		g_TrackingModConfig.DisableDeathByUnknown = data.DisableDeathByUnknown;
		g_TrackingModConfig.DisableKillByGrenade = data.DisableKillByGrenade;
		g_TrackingModConfig.DisableKillByTrap = data.DisableKillByTrap;
		g_TrackingModConfig.DisableKillByAnimal = data.DisableKillByAnimal;
		g_TrackingModConfig.DisableKillByZombie = data.DisableKillByZombie;
		g_TrackingModConfig.DisableKillByCar = data.DisableKillByCar;
		g_TrackingModConfig.DisableKillByWeapon = data.DisableKillByWeapon;
		g_TrackingModConfig.DisableKillByUnarmed = data.DisableKillByUnarmed;
		g_TrackingModConfig.DisableKillByUnknown = data.DisableKillByUnknown;
		g_TrackingModConfig.DisableKillByUnconsciousSuicide = data.DisableKillByUnconsciousSuicide;
		g_TrackingModConfig.TrackPVEKillsInZoneTypes.Clear();
		if (data.TrackPVEKillsInZoneTypes)
		{
			for (i = 0; i < data.TrackPVEKillsInZoneTypes.Count(); i++)
			{
				if (data.TrackPVEKillsInZoneTypes.Get(i) != "")
					g_TrackingModConfig.TrackPVEKillsInZoneTypes.Insert(data.TrackPVEKillsInZoneTypes.Get(i));
			}
		}
		if (g_TrackingModConfig.TrackPVEKillsInZoneTypes.Count() == 0)
			g_TrackingModConfig.TrackPVEKillsInZoneTypes.Insert("everywhere");
		g_TrackingModConfig.ExcludePVPKillWhenBothInPVE = data.ExcludePVPKillWhenBothInPVE;
		g_TrackingModConfig.EnableWebExport = data.EnableWebExport;
		g_TrackingModConfig.WebExportIntervalSeconds = Math.Max(1, data.WebExportIntervalSeconds);
		g_TrackingModConfig.WebExportMaxPlayers = Math.Max(1, Math.Min(data.WebExportMaxPlayers, 1000));
		g_TrackingModConfig.WebExportIncludePlayerIDs = data.WebExportIncludePlayerIDs;
	}

	void RequestAdminConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		TrackingModGeneralAdminData generalData;
		TrackingModPVEAdminData pveData;
		TrackingModPVPAdminData pvpData;

		if (!GetGame().IsDedicatedServer() || type != CallType.Server || !sender)
			return;

		if (!IsSenderTrackingModAdmin(sender))
		{
			TrackingModAdminSaveResponse deniedResponse;
			deniedResponse = new TrackingModAdminSaveResponse();
			deniedResponse.Success = false;
			deniedResponse.Message = "Keine Berechtigung fuer Admin-Konfiguration";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(deniedResponse), true, sender);
			return;
		}

		generalData = BuildGeneralAdminData();
		pveData = BuildPVEAdminData();
		pvpData = BuildPVPAdminData();
		TrackingMod.LogInfo(string.Format("[AdminRPC] Sending admin config to %1 | General AdminIDs=%2 | PVE Categories=%3 | PVP Categories=%4", sender.GetName(), generalData.AdminIDs.Count(), pveData.Categories.Count(), pvpData.Categories.Count()));
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfig", new Param1<TrackingModGeneralAdminData>(generalData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminPVEConfig", new Param1<TrackingModPVEAdminData>(pveData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminPVPConfig", new Param1<TrackingModPVPAdminData>(pvpData), true, sender);
	}

	void SaveAdminConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<TrackingModGeneralAdminData> params;
		TrackingModAdminSaveResponse response;
		TrackingModGeneralAdminData freshData;
		string senderID;

		if (!GetGame().IsDedicatedServer() || type != CallType.Server || !sender)
			return;

		response = new TrackingModAdminSaveResponse();
		senderID = sender.GetPlainId();

		if (!IsSenderTrackingModAdmin(sender))
		{
			response.Success = false;
			response.Message = "Keine Berechtigung";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (!ctx.Read(params) || !params.param1)
		{
			response.Success = false;
			response.Message = "Ungueltige Admin-Daten";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (!params.param1.AdminIDs || params.param1.AdminIDs.Count() == 0)
		{
			response.Success = false;
			response.Message = "Mindestens eine Admin-ID muss erhalten bleiben";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (params.param1.AdminIDs.Find(senderID) == -1)
		{
			response.Success = false;
			response.Message = "Eigene Admin-ID darf nicht entfernt werden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		ApplyGeneralAdminData(params.param1);
		g_TrackingModConfig.SaveConfig();
		ReloadTrackingModConfigs();

		response.Success = true;
		response.Message = "LeaderBoardConfig geladen und angewendet";
		freshData = BuildGeneralAdminData();
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfig", new Param1<TrackingModGeneralAdminData>(freshData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void SaveAdminFullConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<TrackingModGeneralAdminData, TrackingModPVEAdminData, TrackingModPVPAdminData> params;
		TrackingModAdminSaveResponse response;
		TrackingModGeneralAdminData freshGeneralData;
		TrackingModPVEAdminData freshPVEData;
		TrackingModPVPAdminData freshPVPData;
		string senderID;

		if (!GetGame().IsDedicatedServer() || type != CallType.Server || !sender)
			return;

		response = new TrackingModAdminSaveResponse();
		senderID = sender.GetPlainId();

		if (!IsSenderTrackingModAdmin(sender))
		{
			response.Success = false;
			response.Message = "Keine Berechtigung";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (!ctx.Read(params) || !params.param1)
		{
			response.Success = false;
			response.Message = "Ungueltige Admin-Daten";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (!params.param1.AdminIDs || params.param1.AdminIDs.Count() == 0)
		{
			response.Success = false;
			response.Message = "Mindestens eine Admin-ID muss erhalten bleiben";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (params.param1.AdminIDs.Find(senderID) == -1)
		{
			response.Success = false;
			response.Message = "Eigene Admin-ID darf nicht entfernt werden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		ApplyGeneralAdminData(params.param1);
		PVECategoryConfig.GetInstance().ApplyAdminData(params.param2);
		PVPCategoryConfig.GetInstance().ApplyAdminData(params.param3);
		g_TrackingModConfig.SaveConfig();
		ReloadTrackingModConfigs();

		response.Success = true;
		response.Message = "LeaderBoardConfig geladen und angewendet";
		freshGeneralData = BuildGeneralAdminData();
		freshPVEData = BuildPVEAdminData();
		freshPVPData = BuildPVPAdminData();
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfig", new Param1<TrackingModGeneralAdminData>(freshGeneralData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminPVEConfig", new Param1<TrackingModPVEAdminData>(freshPVEData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminPVPConfig", new Param1<TrackingModPVPAdminData>(freshPVPData), true, sender);
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminConfigSaved", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void RequestTrackingModLeaderboard(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<int> params;
		int requestedPage;
		string senderName;
		string senderID;
		TrackingModData data;
		array<Man> players;
		int playerOnlineCounter;
		TrackingModLeaderboardData leaderboardData;
		PVECategoryConfig categoryConfig;
		array<ref PVECategory> categories;
		int catIdx;
		PVECategory category;
		array<ref PlayerDeathData> sortedPVE;
		array<ref PlayerDeathData> sortedPVP;
		int pveDisplayIdx;
		PlayerDeathData displayPlayerData;
		TrackingModLeaderboardPlayerData leaderboardPlayer;
		int pvpDisplayIdx;
		PlayerDeathData pvpDisplayPlayerData;
		TrackingModLeaderboardPlayerData pvpLeaderboardPlayer;
		int sentPveIdx;
		TrackingModLeaderboardPlayerData sentPvePlayerData;
		int sentPvpIdx;
		TrackingModLeaderboardPlayerData sentPvpPlayerData;
		int sentPreviewIdx;
		string sentPreviewCatID;
		string sentPreviewClass;
		int pageSize;
		int totalPVEPlayers;
		int totalPVPPlayers;
		int totalPagesPVE;
		int totalPagesPVP;
		int startIdx;
		int endIdx;
		int pveRequestedPage;
		int pvpRequestedPage;
		PlayerDeathData checkPlayerData;
		int checkIdx;
		int currentTime;
		int lastRequestTime;
		
		if (!ctx.Read(params))
		{
			TrackingMod.LogWarning("RequestTrackingModLeaderboard - failed to read params");
			return;
		}
		requestedPage = params.param1;
		if (requestedPage < 1)
			requestedPage = 1;
		
		senderName = "unknown";
		senderID = "";
		if (sender)
		{
			senderName = sender.GetName();
			senderID = sender.GetPlainId();
		}
		
		currentTime = GetGame().GetTime();
		if (m_LeaderboardRequestTimes && senderID != "")
		{
			if (m_LeaderboardRequestTimes.Contains(senderID))
			{
				lastRequestTime = m_LeaderboardRequestTimes.Get(senderID);
				if (currentTime - lastRequestTime < LEADERBOARD_RATE_LIMIT_MS)
				{
					TrackingMod.LogWarning("RequestTrackingModLeaderboard - rate limited for " + senderName + " (last request: " + (currentTime - lastRequestTime).ToString() + "ms ago)");
					return;
				}
			}
			m_LeaderboardRequestTimes.Set(senderID, currentTime);
		}
		
		TrackingMod.LogDebug("RequestTrackingModLeaderboard received from: " + senderName + " (page: " + requestedPage.ToString() + ")");
		if (!GetGame().IsDedicatedServer() || !sender)
		{
			TrackingMod.LogWarning("Request rejected - not server or no sender");
			return;
		}
		
		pageSize = 25;
		
		if (g_TrackingModConfig)
		{
			if (g_TrackingModConfig.DisablePVELeaderboard && g_TrackingModConfig.DisablePVPLeaderboard)
			{
				TrackingMod.LogWarning("Request rejected - both leaderboards are disabled");
				return;
			}
		}
		
		data = g_TrackingModData;
		if (!data || !data.m_PlayerDataMap)
		{
			TrackingMod.LogWarning("Request rejected - no data or player map");
			return;
		}
		
		players = new array<Man>();
		GetGame().GetPlayers(players);
		playerOnlineCounter = players.Count();
		
		playerOnlineCounter = 0;
		if (data && data.m_PlayerDataMap)
		{
			checkPlayerData = null;
			for (checkIdx = 0; checkIdx < data.m_PlayerDataMap.Count(); checkIdx++)
			{
				checkPlayerData = data.m_PlayerDataMap.GetElement(checkIdx);
				if (checkPlayerData && checkPlayerData.playerIsOnline == 1)
				{
					playerOnlineCounter++;
				}
			}
		}
		
		leaderboardData = new TrackingModLeaderboardData();
		leaderboardData.playerOnlineCounter = playerOnlineCounter;
		if (g_TrackingModConfig)
		{
			leaderboardData.isAdmin = g_TrackingModConfig.IsAdmin(senderID);
			leaderboardData.showPlayerOnlineStatusPVE = g_TrackingModConfig.ShowPlayerOnlineStatusPVE;
			leaderboardData.showPlayerOnlineStatusPVP = g_TrackingModConfig.ShowPlayerOnlineStatusPVP;
			leaderboardData.disablePVPLeaderboard = g_TrackingModConfig.DisablePVPLeaderboard;
			leaderboardData.disablePVELeaderboard = g_TrackingModConfig.DisablePVELeaderboard;
			leaderboardData.survivorIconPathMale = g_TrackingModConfig.SurvivorIconPathMale;
			leaderboardData.survivorIconPathFemale = g_TrackingModConfig.SurvivorIconPathFemale;
			leaderboardData.showShotsFired = g_TrackingModConfig.ShowShotsFired;
			leaderboardData.showShotsHit = g_TrackingModConfig.ShowShotsHit;
			leaderboardData.showHeadshots = g_TrackingModConfig.ShowHeadshots;
			leaderboardData.showHeadshotPercentage = g_TrackingModConfig.ShowHeadshotPercentage;
			leaderboardData.showDistanceTravelled = g_TrackingModConfig.ShowDistanceTravelled;
			leaderboardData.showAccuracy = g_TrackingModConfig.ShowAccuracy;
			if (g_TrackingModConfig.PVEColumns)
				leaderboardData.pveColumns = g_TrackingModConfig.PVEColumns;
			if (g_TrackingModConfig.PVPColumns)
				leaderboardData.pvpColumns = g_TrackingModConfig.PVPColumns;
		}
		
		categoryConfig = PVECategoryConfig.GetInstance();
		if (categoryConfig)
		{
			categories = categoryConfig.GetCategories();
			TrackingMod.LogDebug(string.Format("Loading category previews, found %1 categories", categories.Count()));
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
		else
		{
			TrackingMod.LogCritical("categoryConfig is null!");
		}
		
		sortedPVE = data.GetSortedPVEList();
		sortedPVP = data.GetSortedPVPList();
		
		if (sortedPVE && sortedPVP)
		{
			
			totalPVEPlayers = sortedPVE.Count();
			totalPagesPVE = (totalPVEPlayers + pageSize - 1) / pageSize;
			if (totalPagesPVE < 1)
				totalPagesPVE = 1;
			pveRequestedPage = requestedPage;
			if (pveRequestedPage > totalPagesPVE)
				pveRequestedPage = totalPagesPVE;
			
			startIdx = (pveRequestedPage - 1) * pageSize;
			endIdx = startIdx + pageSize;
			if (endIdx > totalPVEPlayers)
				endIdx = totalPVEPlayers;
			
			leaderboardData.currentPage = pveRequestedPage;
			leaderboardData.totalPages = totalPagesPVE;
			leaderboardData.totalPlayers = totalPVEPlayers;
			
			if (g_TrackingModConfig)
				leaderboardData.maxDisplayCount = g_TrackingModConfig.MaxPVEPlayersDisplay;
			else
				leaderboardData.maxDisplayCount = 100;
			
			leaderboardData.playersOnCurrentPage = endIdx - startIdx;
			
			TrackingMod.LogDebug("PVE leaderboard: page " + pveRequestedPage.ToString() + " of " + totalPagesPVE.ToString() + " (showing " + (endIdx - startIdx).ToString() + " players, " + startIdx.ToString() + "-" + (endIdx - 1).ToString() + " of " + totalPVEPlayers.ToString() + ")");
			
			for (pveDisplayIdx = startIdx; pveDisplayIdx < endIdx; pveDisplayIdx++)
			{
				displayPlayerData = sortedPVE.Get(pveDisplayIdx);
				if (displayPlayerData)
				{
					leaderboardPlayer = CreateLeaderboardPlayerData(displayPlayerData);
					leaderboardData.topPVEPlayers.Insert(leaderboardPlayer);
				}
			}
			
			totalPVPPlayers = sortedPVP.Count();
			totalPagesPVP = (totalPVPPlayers + pageSize - 1) / pageSize;
			if (totalPagesPVP < 1)
				totalPagesPVP = 1;
			pvpRequestedPage = requestedPage;
			if (pvpRequestedPage > totalPagesPVP)
				pvpRequestedPage = totalPagesPVP;
			
			startIdx = (pvpRequestedPage - 1) * pageSize;
			endIdx = startIdx + pageSize;
			if (endIdx > totalPVPPlayers)
				endIdx = totalPVPPlayers;
			
			if (g_TrackingModConfig)
				leaderboardData.maxDisplayCount = g_TrackingModConfig.MaxPVPPlayersDisplay;
			else
				leaderboardData.maxDisplayCount = 24;
			
			leaderboardData.playersOnCurrentPage = endIdx - startIdx;
			
			TrackingMod.LogDebug("PVP leaderboard: page " + pvpRequestedPage.ToString() + " of " + totalPagesPVP.ToString() + " (showing " + (endIdx - startIdx).ToString() + " players, " + startIdx.ToString() + "-" + (endIdx - 1).ToString() + " of " + totalPVPPlayers.ToString() + ")");
			
			for (pvpDisplayIdx = startIdx; pvpDisplayIdx < endIdx; pvpDisplayIdx++)
			{
				pvpDisplayPlayerData = sortedPVP.Get(pvpDisplayIdx);
				if (pvpDisplayPlayerData)
				{
					pvpLeaderboardPlayer = CreateLeaderboardPlayerData(pvpDisplayPlayerData);
					leaderboardData.topPVPPlayers.Insert(pvpLeaderboardPlayer);
				}
			}
		}
		
		TrackingMod.LogRPC("========================================");
		TrackingMod.LogRPC("Sent data:");
		TrackingMod.LogRPC("PlayerOnlineCounter[" + leaderboardData.playerOnlineCounter.ToString() + "]");
		TrackingMod.LogRPC("IsAdmin[" + leaderboardData.isAdmin.ToString() + "]");
		TrackingMod.LogRPC("ShowPlayerOnlineStatusPVE[" + leaderboardData.showPlayerOnlineStatusPVE.ToString() + "]");
		TrackingMod.LogRPC("ShowPlayerOnlineStatusPVP[" + leaderboardData.showPlayerOnlineStatusPVP.ToString() + "]");
		TrackingMod.LogRPC("DisablePVELeaderboard[" + leaderboardData.disablePVELeaderboard.ToString() + "]");
		TrackingMod.LogRPC("DisablePVPLeaderboard[" + leaderboardData.disablePVPLeaderboard.ToString() + "]");
		TrackingMod.LogRPC("TopPVEPlayers[" + leaderboardData.topPVEPlayers.Count().ToString() + "]");
		TrackingMod.LogRPC("TopPVPPlayers[" + leaderboardData.topPVPPlayers.Count().ToString() + "]");
		TrackingMod.LogRPC("CategoryPreviews[" + leaderboardData.categoryPreviews.Count().ToString() + "]");
		
		if (leaderboardData.topPVEPlayers && leaderboardData.topPVEPlayers.Count() > 0)
		{
			TrackingMod.LogRPC("PVE Players:");
			for (sentPveIdx = 0; sentPveIdx < leaderboardData.topPVEPlayers.Count(); sentPveIdx++)
			{
				sentPvePlayerData = leaderboardData.topPVEPlayers.Get(sentPveIdx);
				if (sentPvePlayerData)
				{
					TrackingMod.LogRPC(string.Format("  [%1] PlayerName[%2] PVEPoints[%3] IsOnline[%4] PlayerID[%5]", 
						sentPveIdx + 1, sentPvePlayerData.playerName, sentPvePlayerData.pvePoints, sentPvePlayerData.isOnline, sentPvePlayerData.playerID));
				}
			}
		}
		
		if (leaderboardData.topPVPPlayers && leaderboardData.topPVPPlayers.Count() > 0)
		{
			TrackingMod.LogRPC("PVP Players:");
			for (sentPvpIdx = 0; sentPvpIdx < leaderboardData.topPVPPlayers.Count(); sentPvpIdx++)
			{
				sentPvpPlayerData = leaderboardData.topPVPPlayers.Get(sentPvpIdx);
				if (sentPvpPlayerData)
				{
					TrackingMod.LogRPC(string.Format("  [%1] PlayerName[%2] PVPPoints[%3] IsOnline[%4] PlayerID[%5]", 
						sentPvpIdx + 1, sentPvpPlayerData.playerName, sentPvpPlayerData.pvpPoints, sentPvpPlayerData.isOnline, sentPvpPlayerData.playerID));
				}
			}
		}
		
		if (leaderboardData.categoryPreviews && leaderboardData.categoryPreviews.Count() > 0)
		{
			TrackingMod.LogRPC("Category Previews:");
			for (sentPreviewIdx = 0; sentPreviewIdx < leaderboardData.categoryPreviews.Count(); sentPreviewIdx++)
			{
				sentPreviewCatID = leaderboardData.categoryPreviews.GetKey(sentPreviewIdx);
				sentPreviewClass = leaderboardData.categoryPreviews.GetElement(sentPreviewIdx);
				TrackingMod.LogRPC(string.Format("  CategoryID[%1] PreviewClass[%2]", sentPreviewCatID, sentPreviewClass));
			}
		}
		
		TrackingMod.LogRPC("========================================");
		
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModLeaderboard", new Param1<TrackingModLeaderboardData>(leaderboardData), true, sender);

		if (senderID != "" && data.m_PlayerDataMap && data.m_PlayerDataMap.Contains(senderID))
		{
			PlayerDeathData ownPlayerData;
			TrackingModLeaderboardPlayerData ownStats;
			ownPlayerData = data.m_PlayerDataMap.Get(senderID);
			if (ownPlayerData)
			{
				ownStats = CreateLeaderboardPlayerData(ownPlayerData);
				if (ownStats)
				{
					GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceivePlayerOwnStats", new Param1<TrackingModLeaderboardPlayerData>(ownStats), true, sender);
				}
			}
		}
	}
	
	override TrackingModLeaderboardPlayerData CreateLeaderboardPlayerData(PlayerDeathData playerData)
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

		leaderboardPlayer.shotsFired = playerData.ShotsFired;
		leaderboardPlayer.shotsHit = playerData.ShotsHit;
		leaderboardPlayer.headshots = playerData.Headshots;
		leaderboardPlayer.distanceTravelled = playerData.DistanceTravelled;
		leaderboardPlayer.distanceOnFoot = playerData.DistanceOnFoot;
		leaderboardPlayer.distanceInVehicle = playerData.DistanceInVehicle;
		leaderboardPlayer.totalDeaths = playerData.GetTotalDeaths();
		leaderboardPlayer.suicides = playerData.GetSuicides();
		leaderboardPlayer.accuracy = playerData.GetAccuracy();
		leaderboardPlayer.headshotPercentage = playerData.GetHeadshotPercentage();

		return leaderboardPlayer;
	}
	
	void RequestStyleConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;
		if (!g_TrackingModStyleConfig)
			return;

		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveStyleConfig", new Param1<TrackingModStyleConfig>(g_TrackingModStyleConfig), true, sender);
	}

	void RequestShopConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		if (!g_TrackingModShopConfig)
			return;

		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopConfig", new Param1<TrackingModShopConfig>(g_TrackingModShopConfig), true, sender);
	}

	void PurchaseShopItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<string, int, bool> purchaseParams;
		string itemClassName;
		int amount;
		bool usePvEPoints;
		string senderID;
		TrackingModData data;
		PlayerDeathData buyerData;
		TrackingModAdminSaveResponse response;
		TrackingModShopItem shopItem;
		int i;
		int price;
		int totalCost;
		array<Man> allPlayers;
		PlayerBase buyerPlayer;
		PlayerIdentity pIdentity;
		int pIdx;
		EntityAI spawnedItem;
		int spawned;

		if (type != CallType.Server || !sender)
			return;
		if (!ctx.Read(purchaseParams))
			return;

		itemClassName = purchaseParams.param1;
		amount = purchaseParams.param2;
		usePvEPoints = purchaseParams.param3;
		senderID = sender.GetPlainId();
		response = new TrackingModAdminSaveResponse();

		if (!g_TrackingModShopConfig || !g_TrackingModShopConfig.EnableShop)
		{
			response.Success = false;
			response.Message = "Shop ist deaktiviert";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		shopItem = null;
		for (i = 0; i < g_TrackingModShopConfig.ShopItems.Count(); i++)
		{
			if (g_TrackingModShopConfig.ShopItems.Get(i).ItemClassName == itemClassName)
			{
				shopItem = g_TrackingModShopConfig.ShopItems.Get(i);
				break;
			}
		}

		if (!shopItem)
		{
			response.Success = false;
			response.Message = "Item nicht im Shop gefunden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (amount < 1)
			amount = 1;
		if (amount > shopItem.MaxPerPurchase)
			amount = shopItem.MaxPerPurchase;

		if (usePvEPoints)
			price = shopItem.PricePvE;
		else
			price = shopItem.PricePvP;

		if (price <= 0)
		{
			response.Success = false;
			response.Message = "Nicht kaufbar mit diesem Punktetyp";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		totalCost = price * amount;
		data = g_TrackingModData;
		if (!data)
		{
			response.Success = false;
			response.Message = "Daten nicht verfuegbar";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		buyerData = data.GetPlayerData(senderID);
		if (!buyerData)
		{
			response.Success = false;
			response.Message = "Spielerdaten nicht gefunden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (usePvEPoints && buyerData.PvEPoints < totalCost)
		{
			response.Success = false;
			response.Message = "Nicht genug PvE Points (" + buyerData.PvEPoints.ToString() + "/" + totalCost.ToString() + ")";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}
		if (!usePvEPoints && buyerData.PvPPoints < totalCost)
		{
			response.Success = false;
			response.Message = "Nicht genug PvP Points (" + buyerData.PvPPoints.ToString() + "/" + totalCost.ToString() + ")";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		allPlayers = new array<Man>();
		GetGame().GetPlayers(allPlayers);
		buyerPlayer = null;
		for (pIdx = 0; pIdx < allPlayers.Count(); pIdx++)
		{
			pIdentity = allPlayers.Get(pIdx).GetIdentity();
			if (pIdentity && pIdentity.GetPlainId() == senderID)
			{
				buyerPlayer = PlayerBase.Cast(allPlayers.Get(pIdx));
				break;
			}
		}

		if (!buyerPlayer)
		{
			response.Success = false;
			response.Message = "Spieler nicht online";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		spawned = 0;
		for (i = 0; i < amount; i++)
		{
			spawnedItem = buyerPlayer.GetHumanInventory().CreateInInventory(itemClassName);
			if (!spawnedItem)
			{
				vector bPos;
				vector bDir;
				vector gPos;
				bPos = buyerPlayer.GetPosition();
				bDir = buyerPlayer.GetDirection();
				gPos = bPos + (bDir * 1.5);
				gPos[1] = bPos[1];
				spawnedItem = buyerPlayer.SpawnEntityOnGroundPos(itemClassName, gPos);
			}
			if (spawnedItem)
			{
				if (shopItem.Attachments && shopItem.Attachments.Count() > 0)
					TrackingModRewardHelper.ProcessAttachmentsRecursive(spawnedItem, shopItem.Attachments, itemClassName, 1);
				spawned = spawned + 1;
			}
		}

		if (spawned > 0)
		{
			if (usePvEPoints)
				buyerData.PvEPoints = buyerData.PvEPoints - totalCost;
			else
				buyerData.PvPPoints = buyerData.PvPPoints - totalCost;

			data.SavePlayerData(buyerData, senderID);
			data.MarkLeaderboardDirty();
			data.RebuildSortedLists();

			response.Success = true;
			response.Message = spawned.ToString() + "x " + shopItem.DisplayName + " gekauft fuer " + totalCost.ToString() + " Points";
			TrackingMod.LogInfo("[Shop] " + sender.GetName() + " purchased " + spawned.ToString() + "x " + itemClassName + " for " + totalCost.ToString() + " points");
		}
		else
		{
			response.Success = false;
			response.Message = "Item konnte nicht erstellt werden";
		}

		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveShopPurchaseResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void AdminSearchPlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> searchParams;
		string searchTerm;
		TrackingModData data;
		string resultStr;
		int i;
		string playerID;
		PlayerDeathData pData;
		int matchCount;

		if (type != CallType.Server || !sender)
			return;
		if (!IsSenderTrackingModAdmin(sender))
			return;
		if (!ctx.Read(searchParams))
			return;

		searchTerm = searchParams.param1;
		searchTerm.ToLower();
		data = g_TrackingModData;
		if (!data || !data.m_PlayerDataMap)
			return;

		resultStr = "";
		matchCount = 0;
		for (i = 0; i < data.m_PlayerDataMap.Count(); i++)
		{
			if (matchCount >= 50)
				break;
			playerID = data.m_PlayerDataMap.GetKey(i);
			pData = data.m_PlayerDataMap.GetElement(i);
			if (!pData)
				continue;
			string lowerName = pData.PlayerName;
			lowerName.ToLower();
			if (lowerName.Contains(searchTerm) || playerID.Contains(searchTerm))
			{
				if (resultStr != "")
					resultStr = resultStr + "|";
				resultStr = resultStr + playerID + ":" + pData.PlayerName;
				matchCount = matchCount + 1;
			}
		}

		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminPlayerSearch", new Param1<string>(resultStr), true, sender);
	}

	void AdminWipePlayerData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> wipeParams;
		string targetPlayerID;
		TrackingModData data;
		string filePath;
		TrackingModAdminSaveResponse response;

		if (type != CallType.Server || !sender)
			return;
		if (!IsSenderTrackingModAdmin(sender))
			return;
		if (!ctx.Read(wipeParams))
			return;

		targetPlayerID = wipeParams.param1;
		data = g_TrackingModData;
		response = new TrackingModAdminSaveResponse();

		if (!data || !data.m_PlayerDataMap)
		{
			response.Success = false;
			response.Message = "Daten nicht verfuegbar";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		filePath = GetPlayerFilePath(targetPlayerID);
		if (FileExist(filePath))
		{
			DeleteFile(filePath);
		}

		if (data.m_PlayerDataMap.Contains(targetPlayerID))
		{
			data.m_PlayerDataMap.Remove(targetPlayerID);
		}

		data.MarkLeaderboardDirty();
		data.RebuildSortedLists();
		TrackingMod.LogInfo("[Admin] Player data wiped by " + sender.GetName() + " for player: " + targetPlayerID);

		response.Success = true;
		response.Message = "Spielerdaten fuer " + targetPlayerID + " geloescht";
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void AdminWipePlayerPoints(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> wipeParams;
		string targetPlayerID;
		TrackingModData data;
		PlayerDeathData targetData;
		TrackingModAdminSaveResponse response;

		if (type != CallType.Server || !sender)
			return;
		if (!IsSenderTrackingModAdmin(sender))
			return;
		if (!ctx.Read(wipeParams))
			return;

		targetPlayerID = wipeParams.param1;
		data = g_TrackingModData;
		response = new TrackingModAdminSaveResponse();

		if (!data || !data.m_PlayerDataMap)
		{
			response.Success = false;
			response.Message = "Daten nicht verfuegbar";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		targetData = data.GetPlayerData(targetPlayerID);
		if (!targetData)
		{
			response.Success = false;
			response.Message = "Spieler " + targetPlayerID + " nicht gefunden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		targetData.PvEPoints = 0;
		targetData.PvPPoints = 0;
		data.SavePlayerData(targetData, targetPlayerID);
		data.MarkLeaderboardDirty();
		data.RebuildSortedLists();
		TrackingMod.LogInfo("[Admin] Points wiped by " + sender.GetName() + " for player: " + targetPlayerID);

		response.Success = true;
		response.Message = "Points fuer " + targetData.PlayerName + " auf 0 gesetzt";
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void AdminAwardPoints(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<string, int, bool> awardParams;
		string targetPlayerID;
		int amount;
		bool isPvE;
		TrackingModData data;
		PlayerDeathData targetData;
		TrackingModAdminSaveResponse response;

		if (type != CallType.Server || !sender)
			return;
		if (!IsSenderTrackingModAdmin(sender))
			return;
		if (!ctx.Read(awardParams))
			return;

		targetPlayerID = awardParams.param1;
		amount = awardParams.param2;
		isPvE = awardParams.param3;
		data = g_TrackingModData;
		response = new TrackingModAdminSaveResponse();

		if (!data)
		{
			response.Success = false;
			response.Message = "Daten nicht verfuegbar";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		targetData = data.GetPlayerData(targetPlayerID);
		if (!targetData)
		{
			response.Success = false;
			response.Message = "Spieler " + targetPlayerID + " nicht gefunden";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		if (isPvE)
			targetData.PvEPoints = targetData.PvEPoints + amount;
		else
			targetData.PvPPoints = targetData.PvPPoints + amount;

		data.SavePlayerData(targetData, targetPlayerID);
		data.MarkLeaderboardDirty();
		data.RebuildSortedLists();

		string pointType;
		if (isPvE)
			pointType = "PvE";
		else
			pointType = "PvP";

		TrackingMod.LogInfo("[Admin] " + amount.ToString() + " " + pointType + " points awarded by " + sender.GetName() + " to player: " + targetPlayerID);

		response.Success = true;
		response.Message = amount.ToString() + " " + pointType + " Points an " + targetData.PlayerName + " vergeben";
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	void AdminAwardItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<string, string, int> itemParams;
		string targetPlayerID;
		string className;
		int amount;
		TrackingModData data;
		PlayerDeathData targetData;
		TrackingModAdminSaveResponse response;
		array<Man> allPlayers;
		int i;
		PlayerBase targetPlayer;
		PlayerIdentity targetIdentity;
		EntityAI spawnedItem;
		int spawned;

		if (type != CallType.Server || !sender)
			return;
		if (!IsSenderTrackingModAdmin(sender))
			return;
		if (!ctx.Read(itemParams))
			return;

		targetPlayerID = itemParams.param1;
		className = itemParams.param2;
		amount = itemParams.param3;
		if (amount <= 0)
			amount = 1;

		response = new TrackingModAdminSaveResponse();

		allPlayers = new array<Man>();
		GetGame().GetPlayers(allPlayers);
		targetPlayer = null;
		for (i = 0; i < allPlayers.Count(); i++)
		{
			targetIdentity = allPlayers.Get(i).GetIdentity();
			if (targetIdentity && targetIdentity.GetPlainId() == targetPlayerID)
			{
				targetPlayer = PlayerBase.Cast(allPlayers.Get(i));
				break;
			}
		}

		if (!targetPlayer)
		{
			response.Success = false;
			response.Message = "Spieler " + targetPlayerID + " ist nicht online";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
			return;
		}

		spawned = 0;
		for (i = 0; i < amount; i++)
		{
			spawnedItem = targetPlayer.GetHumanInventory().CreateInInventory(className);
			if (!spawnedItem)
			{
				vector playerPos;
				vector playerDir;
				vector groundPos;
				playerPos = targetPlayer.GetPosition();
				playerDir = targetPlayer.GetDirection();
				groundPos = playerPos + (playerDir * 1.5);
				groundPos[1] = playerPos[1];
				spawnedItem = targetPlayer.SpawnEntityOnGroundPos(className, groundPos);
			}
			if (spawnedItem)
				spawned = spawned + 1;
		}

		TrackingMod.LogInfo("[Admin] " + spawned.ToString() + "x " + className + " awarded by " + sender.GetName() + " to player: " + targetPlayerID);

		response.Success = true;
		response.Message = spawned.ToString() + "x " + className + " an " + targetPlayer.GetIdentity().GetName() + " vergeben";
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveAdminActionResult", new Param1<TrackingModAdminSaveResponse>(response), true, sender);
	}

	override void OnMissionFinish()
	{
		int offlineCount;
		int i;
		PlayerDeathData playerData;
		
		if (GetGame().IsDedicatedServer())
		{
			if (g_TrackingModData && g_TrackingModData.m_PlayerDataMap)
			{
				offlineCount = 0;
				for (i = 0; i < g_TrackingModData.m_PlayerDataMap.Count(); i++)
				{
					playerData = g_TrackingModData.m_PlayerDataMap.GetElement(i);
					if (playerData && playerData.playerIsOnline == 1)
					{
						playerData.playerIsOnline = 0;
						offlineCount++;
					}
				}
				
				if (offlineCount > 0)
				{
					TrackingMod.LogInfo("Server shutting down - Set " + offlineCount.ToString() + " players to offline");
				}
			}
			
			TrackingModData.ForceSave();
			TrackingMod.LogInfo("Server shutting down - Data saved");
		}
		super.OnMissionFinish();
	}
	override void OnEvent(EventType eventTypeId, Param params)
	{
		ChatMessageEventParams chatParams;
		string text;
		string playerName;
		PlayerBase player;
		string playerID;
		int recalculatedCount;
		
		super.OnEvent(eventTypeId, params);
		if (!GetGame().IsDedicatedServer())
			return;
		if (eventTypeId == ChatMessageEventTypeID)
		{
			chatParams = ChatMessageEventParams.Cast(params);
			if (!chatParams)
				return;
			text = chatParams.param3;
			playerName = chatParams.param2;
			if (text == "!rl" || text == "!reload")
			{
				player = GetPlayerByName(playerName);
				if (!player || !player.GetIdentity())
					return;
				playerID = player.GetIdentity().GetPlainId();
				if (g_TrackingModConfig && g_TrackingModConfig.IsAdmin(playerID))
				{
					recalculatedCount = ReloadTrackingModConfigs();
					TrackingMod.LogInfo("Configs reloaded by admin: " + playerName + " (ID: " + playerID + ")");
					player.MessageAction("Config reloaded");
				}
				else
				{
					player.MessageAction("[TrackingMod] Access denied. Admin only.");
				}
			}
		}
	}
	PlayerBase GetPlayerByName(string playerName)
	{
		array<Man> players;
		int i;
		PlayerBase player;
		
		players = new array<Man>();
		GetGame().GetPlayers(players);
		for (i = 0; i < players.Count(); i++)
		{
			player = PlayerBase.Cast(players[i]);
			if (player && player.GetIdentity())
			{
				if (player.GetIdentity().GetName() == playerName)
					return player;
			}
		}
		return null;
	}
	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		TrackingModData data;
		array<Man> connectedPlayers;
		map<string, bool> connectedPlayerIDs;
		int connectedIdx;
		PlayerBase connectedPlayer;
		string connectedID;
		int offlineCount;
		int checkIdx;
		PlayerDeathData checkPlayerData;
		
		super.PlayerDisconnected(player, identity, uid);
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		data = TrackingModData.LoadData();
		if (!data || !data.m_PlayerDataMap)
			return;
		
		connectedPlayers = new array<Man>();
		GetGame().GetPlayers(connectedPlayers);
		
		connectedPlayerIDs = new map<string, bool>();
		for (connectedIdx = 0; connectedIdx < connectedPlayers.Count(); connectedIdx++)
		{
			connectedPlayer = PlayerBase.Cast(connectedPlayers.Get(connectedIdx));
			if (connectedPlayer && connectedPlayer.GetIdentity())
			{
				connectedID = connectedPlayer.GetIdentity().GetPlainId();
				connectedPlayerIDs.Set(connectedID, true);
			}
		}
		
		offlineCount = 0;
		for (checkIdx = 0; checkIdx < data.m_PlayerDataMap.Count(); checkIdx++)
		{
			checkPlayerData = data.m_PlayerDataMap.GetElement(checkIdx);
			if (checkPlayerData && checkPlayerData.playerIsOnline == 1)
			{
				if (!connectedPlayerIDs.Contains(checkPlayerData.PlayerID))
				{
					checkPlayerData.playerIsOnline = 0;
					data.SavePlayerData(checkPlayerData, checkPlayerData.PlayerID);
					offlineCount++;
					TrackingMod.LogDebug(string.Format("PlayerDisconnected - Set player offline: %1 (ID: %2) - logout uid: %3", checkPlayerData.PlayerName, checkPlayerData.PlayerID, uid));
				}
			}
		}
	}
	
	override void OnClientRespawnEvent(PlayerIdentity identity, PlayerBase player)
	{
		string plainID;
		string playerName;
		string typeName;
		string survivorType;
		TrackingModData data;
		PlayerDeathData playerData;
		
		super.OnClientRespawnEvent(identity, player);
		
		if (player && identity)
		{
			plainID = identity.GetPlainId();
			playerName = identity.GetName();
			typeName = player.GetType();
			survivorType = player.GetSurvivorType();
			TrackingMod.LogDebug(string.Format("OnClientRespawnEvent - Player: %1 (PlainID: %2), Type: %3, SurvivorType: %4", playerName, plainID, typeName, survivorType));
			
			data = TrackingModData.LoadData();
			playerData = data.GetPlayerData(plainID, playerName);
			if (playerData)
			{
				playerData.playerIsOnline = 1;
				playerData.survivorType = survivorType;
				data.UpdateLastLoginDate(playerData);
				data.SavePlayerData(playerData, plainID);
			}
		}
	}
	
	int ReloadTrackingModConfigs()
	{
		array<Man> currentPlayers;
		int updatedCount;
		int i;
		PlayerBase player;
		string plainID;
		string playerName;
		PlayerDeathData playerData;
		
		if (!GetGame().IsDedicatedServer())
			return 0;
		TrackingMod.LogInfo("Reloading configs...");
		g_TrackingModConfig = TrackingModConfig.LoadConfig();
		if (g_TrackingModConfig)
		{
			TrackingMod.LogInfo("Config reloaded - MaxPVEPlayersDisplay: " + g_TrackingModConfig.MaxPVEPlayersDisplay.ToString() + ", MaxPVPPlayersDisplay: " + g_TrackingModConfig.MaxPVPPlayersDisplay.ToString() + ", ShowPlayerOnlineStatusPVE: " + g_TrackingModConfig.ShowPlayerOnlineStatusPVE.ToString() + ", ShowPlayerOnlineStatusPVP: " + g_TrackingModConfig.ShowPlayerOnlineStatusPVP.ToString());
		}
		g_TrackingModRewardConfig = TrackingModRewardConfig.LoadConfig();
		if (g_TrackingModRewardConfig)
		{
			TrackingMod.LogInfo("Reward config reloaded - EnableMilestoneRewards: " + g_TrackingModRewardConfig.EnableMilestoneRewards.ToString());
		}
		PVECategoryConfig.GetInstance(true);
		PVPCategoryConfig.GetInstance(true);
		DeathCategoryConfig.GetInstance(true);
		
		if (g_TrackingModData)
		{
			TrackingMod.LogInfo("Reloading player data from JSON files...");
			g_TrackingModData.m_PlayerDataMap.Clear();
			g_TrackingModData.LoadAllPlayerFiles();
			
			TrackingMod.LogInfo("Updating online status for currently connected players...");
			currentPlayers = new array<Man>();
			GetGame().GetPlayers(currentPlayers);
			updatedCount = 0;
			for (i = 0; i < currentPlayers.Count(); i++)
			{
				player = PlayerBase.Cast(currentPlayers.Get(i));
				if (player && player.GetIdentity())
				{
					plainID = player.GetIdentity().GetPlainId();
					playerName = player.GetIdentity().GetName();
					playerData = g_TrackingModData.GetPlayerData(plainID, playerName);
					if (playerData)
					{
						playerData.playerIsOnline = 1;
						playerData.survivorType = player.GetSurvivorType();
						g_TrackingModData.UpdateLastLoginDate(playerData);
						g_TrackingModData.SavePlayerData(playerData, plainID);
						updatedCount++;
					}
				}
			}
			if (updatedCount > 0)
			{
				TrackingMod.LogInfo(string.Format("Updated online status for %1 currently connected players", updatedCount));
			}
			
			g_TrackingModData.MarkLeaderboardDirty();
			g_TrackingModData.RebuildSortedLists();
			g_TrackingModData.ExportWebLeaderboard();
		}
		
		TrackingMod.LogInfo("Configs reloaded successfully!");
		return 1;
	}
	
	void ClaimTrackingModReward(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		PlayerBase player;
		PlayerBase tempPlayer;
		string plainID;
		string senderName;
		TrackingModData data;
		PlayerDeathData playerData;
		TrackingModRewardClaimResponse response;
		array<ref TrackingModRewardSection> rewardSections;
		bool useSectionSystem;
		array<ref TrackingModCurrencyReward> currencyRewards;
		int currencyMin;
		int currencyMax;
		int sectionIdx;
		TrackingModRewardSection section;
		float totalWeight;
		float randomRoll;
		float currentWeight;
		TrackingModRewardSection selectedSection;
		bool sectionSuccess;
		int maxSectionRetries;
		int sectionRetryCount;
		int sectionItemMin;
		int sectionItemMax;
		int itemsToPick;
		array<ref TrackingModRewardItem> sectionItemsToSpawn;
		TrackingModRewardItem sectionItem;
		TrackingModRewardItem selectedSectionItem;
		int sectionPickedCount;
		int sectionJ;
		int i;
		int itemRetryCount;
		int maxItemRetries;
		array<ref TrackingModRewardItem> availableSectionItems;
		array<ref TrackingModRewardItem> tempAvailableItems;
		bool sectionHasItems;
		array<ref TrackingModCurrencyReward> currenciesToGive;
		array<Man> allPlayers;
		int calculatedPendingRewards;
		TrackingModMilestoneResult milestoneResult;
		string milestoneCategoryID;
		int milestoneValue;
		string rewardConfigFileName;
		TrackingModMilestoneRewardConfig milestoneRewardConfig;
		string milestoneStr;
		bool spawnSuccess;
		string rewardDisplayName;
		string rewardNotificationText;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		if (sender)
			senderName = sender.GetName();
		else
			senderName = "NULL";
		TrackingMod.LogInfo("[ClaimTrackingModReward] RPC received from player: " + senderName);
		
		if (!sender)
		{
			TrackingMod.LogWarning("[ClaimTrackingModReward] Invalid sender");
			return;
		}
		
		plainID = sender.GetPlainId();
		player = null;
		allPlayers = new array<Man>();
		GetGame().GetPlayers(allPlayers);
		for (i = 0; i < allPlayers.Count(); i++)
		{
			tempPlayer = PlayerBase.Cast(allPlayers.Get(i));
			if (tempPlayer && tempPlayer.GetIdentity())
			{
				if (tempPlayer.GetIdentity().GetPlainId() == plainID)
				{
					player = tempPlayer;
					break;
				}
			}
		}
		
		if (!player)
		{
			TrackingMod.LogWarning("[ClaimTrackingModReward] Player not found for plainID: " + plainID);
			response = new TrackingModRewardClaimResponse();
			response.success = false;
			response.remainingRewards = 0;
			response.message = "Player not found";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
			return;
		}
		
		data = TrackingModData.LoadData();
		playerData = data.GetPlayerData(plainID, sender.GetName());
		
		TrackingMod.LogInfo("[ClaimTrackingModReward] Player data retrieved - PlainID: " + plainID + ", Name: " + sender.GetName());
		
		if (!playerData)
		{
			TrackingMod.LogWarning("[ClaimTrackingModReward] Player data not found for " + sender.GetName());
			response = new TrackingModRewardClaimResponse();
			response.success = false;
			response.remainingRewards = 0;
			response.message = "Player data not found";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
			return;
		}
		
		calculatedPendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
		TrackingMod.LogInfo("[ClaimTrackingModReward] Player AvailableMilestones count: " + calculatedPendingRewards.ToString());
		
		if (!g_TrackingModConfig)
		{
			TrackingMod.LogWarning("[ClaimTrackingModReward] Config not loaded");
			response = new TrackingModRewardClaimResponse();
			response.success = false;
			response.remainingRewards = calculatedPendingRewards;
			response.message = "Config not loaded";
			GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
			return;
		}
		
		if (g_TrackingModRewardConfig && g_TrackingModRewardConfig.EnableMilestoneRewards)
		{
			calculatedPendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
			TrackingMod.LogInfo("[ClaimTrackingModReward] Calculated pending rewards from milestones: " + calculatedPendingRewards.ToString());
			
			if (calculatedPendingRewards <= 0)
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] Player " + sender.GetName() + " tried to claim rewards but has no pending milestone rewards");
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = 0;
				response.message = "No pending milestone rewards";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
			
			milestoneResult = TrackingModMilestoneHelper.GetNextAvailableMilestone(playerData);
			milestoneCategoryID = milestoneResult.CategoryID;
			milestoneValue = milestoneResult.Milestone;
			
			if (milestoneCategoryID == "" || milestoneValue == 0)
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] Could not find available milestone to claim");
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = calculatedPendingRewards;
				response.message = "No available milestone found";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
			
			milestoneStr = milestoneValue.ToString();
			rewardConfigFileName = "";
			
			rewardConfigFileName = g_TrackingModRewardConfig.GetRewardConfigFileName(milestoneCategoryID, milestoneStr, true);
			
			if (rewardConfigFileName == "")
			{
				rewardConfigFileName = g_TrackingModRewardConfig.GetRewardConfigFileName(milestoneCategoryID, milestoneStr, false);
			}
			
			if (rewardConfigFileName == "")
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] No reward config file found for milestone: " + milestoneCategoryID + " - " + milestoneValue.ToString());
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = calculatedPendingRewards;
				response.message = "Reward config file not found for milestone";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
			
			milestoneRewardConfig = TrackingModMilestoneRewardLoader.LoadRewardConfig(rewardConfigFileName);
			if (!milestoneRewardConfig || !milestoneRewardConfig.RewardItems || milestoneRewardConfig.RewardItems.Count() == 0)
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] Failed to load reward config or config has no items: " + rewardConfigFileName);
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = calculatedPendingRewards;
				response.message = "Failed to load reward config file";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
			
			rewardSections = milestoneRewardConfig.RewardItems;
			useSectionSystem = rewardSections && rewardSections.Count() > 0;
			currencyRewards = milestoneRewardConfig.CurrencyRewards;
			currencyMin = milestoneRewardConfig.CurrencyMin;
			currencyMax = milestoneRewardConfig.CurrencyMax;
			
			if (currencyMin < 0)
				currencyMin = 0;
			if (currencyMax < currencyMin)
				currencyMax = currencyMin;
			
			bool hasCurrencyOnly = !useSectionSystem && currencyRewards && currencyRewards.Count() > 0;

			if (!useSectionSystem && !hasCurrencyOnly)
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] Reward config has no sections and no currency: " + rewardConfigFileName);
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = calculatedPendingRewards;
				response.message = "Reward config has no sections and no currency";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
		}
		
		selectedSection = null;
		sectionSuccess = false;
		maxSectionRetries = 10;
		sectionRetryCount = 0;
		rewardDisplayName = "";

		if (!useSectionSystem)
		{
			sectionSuccess = true;
			TrackingMod.LogInfo("[ClaimTrackingModReward] Currency-only reward, skipping section selection");
		}

		while (!sectionSuccess && sectionRetryCount < maxSectionRetries)
		{
			sectionRetryCount++;
			selectedSection = null;
			totalWeight = 0.0;
			
			for (sectionIdx = 0; sectionIdx < rewardSections.Count(); sectionIdx++)
			{
				section = rewardSections.Get(sectionIdx);
				if (!section || !section.LootItems || section.LootItems.Count() == 0)
					continue;
				totalWeight = totalWeight + section.SpawnChance;
			}
			
			if (totalWeight > 0.0)
			{
				randomRoll = Math.RandomFloat(0.0, totalWeight);
				currentWeight = 0.0;
				for (sectionIdx = 0; sectionIdx < rewardSections.Count(); sectionIdx++)
				{
					section = rewardSections.Get(sectionIdx);
					if (!section || !section.LootItems || section.LootItems.Count() == 0)
						continue;
					
					currentWeight = currentWeight + section.SpawnChance;
					if (randomRoll <= currentWeight)
					{
						selectedSection = section;
						TrackingMod.LogInfo("[ClaimTrackingModReward] Section " + section.Name + " selected (SpawnChance: " + section.SpawnChance.ToString() + "%, total weight: " + totalWeight.ToString() + ", rolled: " + randomRoll.ToString() + ", retry: " + sectionRetryCount.ToString() + ")");
						break;
					}
				}
			}
			
			if (selectedSection)
			{
				section = selectedSection;
				if (section.Name != "")
				{
					rewardDisplayName = section.Name;
				}
				sectionItemMin = section.ItemsMin;
				sectionItemMax = section.ItemsMax;
				if (sectionItemMin < 0)
					sectionItemMin = 0;
				if (sectionItemMax < sectionItemMin)
					sectionItemMax = sectionItemMin;
				
				itemsToPick = sectionItemMin;
				if (sectionItemMax > sectionItemMin)
					itemsToPick = Math.RandomInt(sectionItemMin, sectionItemMax + 1);
				
				if (itemsToPick > section.LootItems.Count())
					itemsToPick = section.LootItems.Count();
				
				sectionItemsToSpawn = new array<ref TrackingModRewardItem>;
				itemRetryCount = 0;
				maxItemRetries = 20;
				
				while (sectionItemsToSpawn.Count() == 0 && itemRetryCount < maxItemRetries)
				{
					itemRetryCount++;
					sectionItemsToSpawn.Clear();
					
					if (itemsToPick > 0)
					{
						availableSectionItems = new array<ref TrackingModRewardItem>;
						for (i = 0; i < section.LootItems.Count(); i++)
						{
							sectionItem = section.LootItems.Get(i);
							if (sectionItem && sectionItem.ItemClassName != "")
							{
								availableSectionItems.Insert(sectionItem);
							}
						}
						
						sectionPickedCount = 0;
						tempAvailableItems = new array<ref TrackingModRewardItem>;
						for (i = 0; i < availableSectionItems.Count(); i++)
						{
							tempAvailableItems.Insert(availableSectionItems.Get(i));
						}
						
						while (sectionPickedCount < itemsToPick && tempAvailableItems.Count() > 0)
						{
							selectedSectionItem = TrackingModRewardHelper.SelectRandomRewardItem(tempAvailableItems);
							if (selectedSectionItem)
							{
								sectionItemsToSpawn.Insert(selectedSectionItem);
								for (sectionJ = tempAvailableItems.Count() - 1; sectionJ >= 0; sectionJ--)
								{
									if (tempAvailableItems.Get(sectionJ) == selectedSectionItem)
									{
										tempAvailableItems.Remove(sectionJ);
										break;
									}
								}
								sectionPickedCount = sectionPickedCount + 1;
							}
							else
							{
								break;
							}
						}
					}
				}
				
				sectionHasItems = sectionItemsToSpawn.Count() > 0;
				if (sectionHasItems)
				{
					spawnSuccess = TrackingModRewardHelper.AddItemsToPlayerInventory(player, sectionItemsToSpawn);
					if (spawnSuccess)
					{
						TrackingMod.LogInfo("[ClaimTrackingModReward] Added " + sectionItemsToSpawn.Count().ToString() + " item(s) directly to player inventory for section " + section.Name);
						sectionSuccess = true;
					}
					else
					{
						TrackingMod.LogWarning("[ClaimTrackingModReward] Failed to add items to player inventory for section " + section.Name + " (no space available). Retrying section selection...");
						sectionSuccess = false;
					}
				}
				else
				{
					TrackingMod.LogWarning("[ClaimTrackingModReward] Selected section " + section.Name + " has no items to spawn! Retrying section selection...");
					sectionSuccess = false;
				}
			}
			else
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] No section was selected! Retrying...");
				sectionSuccess = false;
			}
		}
		
		if (!sectionSuccess && sectionRetryCount >= maxSectionRetries)
		{
			if (currencyRewards && currencyRewards.Count() > 0)
			{
				TrackingMod.LogWarning("[ClaimTrackingModReward] Section selection failed but currency rewards available — giving currency only");
			}
			else
			{
				TrackingMod.LogCritical("[ClaimTrackingModReward] Failed to get a section with items after " + maxSectionRetries.ToString() + " retries! Not consuming reward.");
				if (player && player.GetIdentity())
				{
					rewardNotificationText = "Failed to claim reward. Your inventory may be full. Please make space and try again.";
					if (rewardDisplayName != "")
					{
						rewardNotificationText = "Failed to claim reward \"" + rewardDisplayName + "\". Your inventory may be full. Please make space and try again.";
					}
					NotificationSystem.Create(new StringLocaliser("Reward Claim Failed"), new StringLocaliser(rewardNotificationText), "Ninjins_Tracking_Mod/gui/error.edds", ARGB(255, 255, 0, 0), 5.0, player.GetIdentity());
				}
				calculatedPendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
				response = new TrackingModRewardClaimResponse();
				response.success = false;
				response.remainingRewards = calculatedPendingRewards;
				response.message = "Failed to select reward section after retries";
				GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
				return;
			}
		}
		
		if (currencyRewards && currencyRewards.Count() > 0)
		{
			currenciesToGive = TrackingModRewardHelper.SelectCurrencyRewards(currencyRewards, currencyMin, currencyMax);
			if (currenciesToGive && currenciesToGive.Count() > 0)
			{
				TrackingModRewardHelper.AddCurrencyToPlayerInventory(player, currenciesToGive);
				TrackingMod.LogInfo("[ClaimTrackingModReward] Added " + currenciesToGive.Count().ToString() + " currency rewards to player inventory (CurrencyMin: " + currencyMin.ToString() + ", CurrencyMax: " + currencyMax.ToString() + ")");
			}
		}
		
		TrackingModMilestoneHelper.MarkMilestoneAsClaimed(playerData, milestoneCategoryID, milestoneValue);
		calculatedPendingRewards = TrackingModMilestoneHelper.CalculatePendingRewards(playerData);
		data.SavePlayerData(playerData, plainID);
		TrackingMod.LogInfo("[ClaimTrackingModReward] Successfully claimed milestone reward for " + sender.GetName() + " (Category: " + milestoneCategoryID + ", Milestone: " + milestoneValue.ToString() + ", remaining: " + calculatedPendingRewards.ToString() + ")");
		
		if (player && player.GetIdentity())
		{
			rewardNotificationText = "Reward items have been added to your inventory.";
			if (rewardDisplayName != "")
			{
				rewardNotificationText = "Reward \"" + rewardDisplayName + "\" has been added to your inventory.";
			}
			NotificationSystem.Create(new StringLocaliser("Reward Claimed"), new StringLocaliser(rewardNotificationText), "Ninjins_Tracking_Mod/gui/success.edds", ARGB(255, 0, 255, 0), 3.0, player.GetIdentity());
		}
		
		response = new TrackingModRewardClaimResponse();
		response.success = true;
		response.remainingRewards = calculatedPendingRewards;
		response.message = "Reward claimed successfully";
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "ReceiveTrackingModRewardClaim", new Param1<TrackingModRewardClaimResponse>(response), true, sender);
		TrackingMod.LogInfo("[ClaimTrackingModReward] Response sent to client - success: true, remaining: " + calculatedPendingRewards.ToString());
	}
	
}
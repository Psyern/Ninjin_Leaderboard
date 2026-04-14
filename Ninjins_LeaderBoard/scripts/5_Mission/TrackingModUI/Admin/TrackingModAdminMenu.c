class TrackingModAdminMenu: ScriptViewMenu
{
	protected static ref TrackingModAdminMenu s_Instance;

	protected ref TrackingModAdminMenuController m_MenuController;
	protected ref TrackingModLeaderboardData m_LeaderboardData;
	protected ref TrackingModGeneralAdminData m_GeneralConfigData;
	protected ref array<string> m_ZoneTypeOptions;
	protected int m_ActiveTab = 0;
	protected bool m_HasRequestedConfig;

	protected ButtonWidget m_BtnTabGeneral;
	protected ButtonWidget m_BtnTabPVE;
	protected ButtonWidget m_BtnTabPVP;
	protected ButtonWidget m_BtnTabMilestones;
	protected ButtonWidget m_BtnTabRewards;
	protected ButtonWidget m_BtnSaveApply;
	protected ButtonWidget m_BtnClose;

	protected Widget m_TabGeneralPanel;
	protected Widget m_TabPVEPanel;
	protected Widget m_TabPVPPanel;
	protected Widget m_TabMilestonesPanel;
	protected Widget m_TabRewardsPanel;

	protected TextListboxWidget m_AdminIdsList;
	protected EditBoxWidget m_EditAdminId;
	protected CheckBoxWidget m_CheckEnablePVE;
	protected CheckBoxWidget m_CheckEnablePVP;
	protected EditBoxWidget m_EditMaxPVE;
	protected EditBoxWidget m_EditMaxPVP;
	protected CheckBoxWidget m_CheckShowOnlinePVE;
	protected CheckBoxWidget m_CheckShowOnlinePVP;
	protected EditBoxWidget m_EditDeleteDays;
	protected CheckBoxWidget m_CheckUseUTC;
	protected CheckBoxWidget m_CheckEnableRewardSystem;
	protected CheckBoxWidget m_CheckDeathSuicide;
	protected CheckBoxWidget m_CheckDeathGrenade;
	protected CheckBoxWidget m_CheckDeathTrap;
	protected CheckBoxWidget m_CheckDeathZombie;
	protected CheckBoxWidget m_CheckDeathAnimal;
	protected CheckBoxWidget m_CheckDeathAI;
	protected CheckBoxWidget m_CheckDeathCar;
	protected CheckBoxWidget m_CheckDeathWeapon;
	protected CheckBoxWidget m_CheckDeathUnarmed;
	protected CheckBoxWidget m_CheckDeathUnknown;
	protected CheckBoxWidget m_CheckKillGrenade;
	protected CheckBoxWidget m_CheckKillTrap;
	protected CheckBoxWidget m_CheckKillAnimal;
	protected CheckBoxWidget m_CheckKillZombie;
	protected CheckBoxWidget m_CheckKillCar;
	protected CheckBoxWidget m_CheckKillWeapon;
	protected CheckBoxWidget m_CheckKillUnarmed;
	protected CheckBoxWidget m_CheckKillUnknown;
	protected CheckBoxWidget m_CheckKillUnconscious;
	protected CheckBoxWidget m_CheckExcludePVPInPVE;
	protected TextListboxWidget m_ZoneTypesList;
	protected CheckBoxWidget m_CheckEnableWebExport;
	protected EditBoxWidget m_EditExportInterval;
	protected EditBoxWidget m_EditExportPlayers;
	protected CheckBoxWidget m_CheckExportPlayerIDs;
	protected EditBoxWidget m_EditPVEPenalty;
	protected TextListboxWidget m_PVECategoriesList;
	protected EditBoxWidget m_EditPVECategoryId;
	protected EditBoxWidget m_EditPVEPreview;
	protected EditBoxWidget m_EditPVEEntryValue;
	protected TextListboxWidget m_PVEEntriesList;
	protected EditBoxWidget m_EditPVPPenalty;
	protected TextListboxWidget m_PVPCategoriesList;
	protected EditBoxWidget m_EditPVPCategoryId;
	protected EditBoxWidget m_EditPVPEntryValue;
	protected TextListboxWidget m_PVPEntriesList;
	protected ref TrackingModPVEAdminData m_PVEConfigData;
	protected ref TrackingModPVPAdminData m_PVPConfigData;
	protected int m_SelectedPVECategoryIndex;
	protected int m_SelectedPVPCategoryIndex;

	protected ButtonWidget m_BtnTabPlayers;
	protected Widget m_TabPlayersPanel;
	protected EditBoxWidget m_EditPlayerSearch;
	protected ButtonWidget m_BtnSearchPlayer;
	protected TextListboxWidget m_PlayerResultList;
	protected TextWidget m_PlayerSelectedLabel;
	protected ButtonWidget m_BtnWipePlayerData;
	protected ButtonWidget m_BtnWipePlayerPoints;
	protected EditBoxWidget m_EditAwardPoints;
	protected ButtonWidget m_BtnAwardPVEPoints;
	protected ButtonWidget m_BtnAwardPVPPoints;
	protected EditBoxWidget m_EditAwardItemClass;
	protected EditBoxWidget m_EditAwardItemAmount;
	protected ButtonWidget m_BtnAwardItem;
	protected TextWidget m_PlayerActionStatus;
	protected string m_SelectedPlayerID;
	protected string m_SelectedPlayerName;

	void TrackingModAdminMenu()
	{
		s_Instance = this;
		m_MenuController = TrackingModAdminMenuController.Cast(m_Controller);
		m_ZoneTypeOptions = new array<string>();
		m_ZoneTypeOptions.Insert("everywhere");
		m_ZoneTypeOptions.Insert("pvp");
		m_ZoneTypeOptions.Insert("pve");
		m_ZoneTypeOptions.Insert("visual");
		m_ZoneTypeOptions.Insert("raid");
		m_GeneralConfigData = new TrackingModGeneralAdminData();
		m_PVEConfigData = new TrackingModPVEAdminData();
		m_PVPConfigData = new TrackingModPVPAdminData();
		m_SelectedPVECategoryIndex = -1;
		m_SelectedPVPCategoryIndex = -1;
	}

	void ~TrackingModAdminMenu()
	{
		if (s_Instance == this)
			s_Instance = null;
		UnlockControls();
	}

	static TrackingModAdminMenu GetInstance()
	{
		return s_Instance;
	}

	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		CacheWidgets();
		LockControls();
		SetActiveTab(0);
		SetFocus(GetLayoutRoot());
		GetGame().GetMission().GetHud().Show(false);
		FillZoneTypeList();
		RequestConfigIfPossible();
	}

	void SetLeaderboardData(TrackingModLeaderboardData data)
	{
		m_LeaderboardData = data;
		if (!m_MenuController)
			return;

		if (m_LeaderboardData && m_LeaderboardData.isAdmin)
			m_MenuController.AdminInfoText = "Admin erkannt - Servervalidierung aktiv";
		else
			m_MenuController.AdminInfoText = "Nur fuer Server-Admins";

		m_MenuController.NotifyPropertiesChanged({"AdminInfoText"});
		RequestConfigIfPossible();
	}

	void RequestConfigIfPossible()
	{
		PlayerBase player;

		if (m_HasRequestedConfig)
			return;
		if (!m_LeaderboardData || !m_LeaderboardData.isAdmin)
			return;

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		m_HasRequestedConfig = true;
		SetStatus("Lade Admin-Konfiguration...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "RequestAdminConfig", null, true, player.GetIdentity());
	}

	void ApplyGeneralConfigData(TrackingModGeneralAdminData data)
	{
		if (!data)
			return;

		TrackingModUI.LogRPC(string.Format("[AdminMenu] ApplyGeneralConfigData | AdminIDs=%1 | MaxPVE=%2 | MaxPVP=%3", data.AdminIDs.Count(), data.MaxPVEPlayersDisplay, data.MaxPVPPlayersDisplay));

		m_GeneralConfigData = data;
		FillAdminList();
		FillZoneTypeList();

		if (m_CheckEnablePVE)
			m_CheckEnablePVE.SetChecked(!data.DisablePVELeaderboard);
		if (m_CheckEnablePVP)
			m_CheckEnablePVP.SetChecked(!data.DisablePVPLeaderboard);
		if (m_EditMaxPVE)
			m_EditMaxPVE.SetText(data.MaxPVEPlayersDisplay.ToString());
		if (m_EditMaxPVP)
			m_EditMaxPVP.SetText(data.MaxPVPPlayersDisplay.ToString());
		if (m_CheckShowOnlinePVE)
			m_CheckShowOnlinePVE.SetChecked(data.ShowPlayerOnlineStatusPVE);
		if (m_CheckShowOnlinePVP)
			m_CheckShowOnlinePVP.SetChecked(data.ShowPlayerOnlineStatusPVP);
		if (m_EditDeleteDays)
			m_EditDeleteDays.SetText(data.DeletePlayerFilesOlderThanDays.ToString());
		if (m_CheckUseUTC)
			m_CheckUseUTC.SetChecked(data.UseUTCForDates);
		if (m_CheckEnableRewardSystem)
			m_CheckEnableRewardSystem.SetChecked(data.EnableRewardSystem);
		if (m_CheckDeathSuicide)
			m_CheckDeathSuicide.SetChecked(data.DisableDeathBySuicide);
		if (m_CheckDeathGrenade)
			m_CheckDeathGrenade.SetChecked(data.DisableDeathByGrenade);
		if (m_CheckDeathTrap)
			m_CheckDeathTrap.SetChecked(data.DisableDeathByTrap);
		if (m_CheckDeathZombie)
			m_CheckDeathZombie.SetChecked(data.DisableDeathByZombie);
		if (m_CheckDeathAnimal)
			m_CheckDeathAnimal.SetChecked(data.DisableDeathByAnimal);
		if (m_CheckDeathAI)
			m_CheckDeathAI.SetChecked(data.DisableDeathByAI);
		if (m_CheckDeathCar)
			m_CheckDeathCar.SetChecked(data.DisableDeathByCar);
		if (m_CheckDeathWeapon)
			m_CheckDeathWeapon.SetChecked(data.DisableDeathByWeapon);
		if (m_CheckDeathUnarmed)
			m_CheckDeathUnarmed.SetChecked(data.DisableDeathByUnarmed);
		if (m_CheckDeathUnknown)
			m_CheckDeathUnknown.SetChecked(data.DisableDeathByUnknown);
		if (m_CheckKillGrenade)
			m_CheckKillGrenade.SetChecked(data.DisableKillByGrenade);
		if (m_CheckKillTrap)
			m_CheckKillTrap.SetChecked(data.DisableKillByTrap);
		if (m_CheckKillAnimal)
			m_CheckKillAnimal.SetChecked(data.DisableKillByAnimal);
		if (m_CheckKillZombie)
			m_CheckKillZombie.SetChecked(data.DisableKillByZombie);
		if (m_CheckKillCar)
			m_CheckKillCar.SetChecked(data.DisableKillByCar);
		if (m_CheckKillWeapon)
			m_CheckKillWeapon.SetChecked(data.DisableKillByWeapon);
		if (m_CheckKillUnarmed)
			m_CheckKillUnarmed.SetChecked(data.DisableKillByUnarmed);
		if (m_CheckKillUnknown)
			m_CheckKillUnknown.SetChecked(data.DisableKillByUnknown);
		if (m_CheckKillUnconscious)
			m_CheckKillUnconscious.SetChecked(data.DisableKillByUnconsciousSuicide);
		if (m_CheckExcludePVPInPVE)
			m_CheckExcludePVPInPVE.SetChecked(data.ExcludePVPKillWhenBothInPVE);
		if (m_CheckEnableWebExport)
			m_CheckEnableWebExport.SetChecked(data.EnableWebExport);
		if (m_EditExportInterval)
			m_EditExportInterval.SetText(data.WebExportIntervalSeconds.ToString());
		if (m_EditExportPlayers)
			m_EditExportPlayers.SetText(data.WebExportMaxPlayers.ToString());
		if (m_CheckExportPlayerIDs)
			m_CheckExportPlayerIDs.SetChecked(data.WebExportIncludePlayerIDs);

		if (m_EditAdminId)
			m_EditAdminId.SetText("");

		SetStatus("Admin-Konfiguration geladen");
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

	void ApplyPVEConfigData(TrackingModPVEAdminData data)
	{
		int i;
		PVECategory category;

		if (data)
			TrackingModUI.LogRPC(string.Format("[AdminMenu] ApplyPVEConfigData | Penalty=%1 | Categories=%2", data.PVEDeathPenaltyPoints, data.Categories.Count()));

		m_PVEConfigData = new TrackingModPVEAdminData();
		if (data)
		{
			m_PVEConfigData.PVEDeathPenaltyPoints = data.PVEDeathPenaltyPoints;
			for (i = 0; i < data.Categories.Count(); i++)
			{
				category = ClonePVECategory(data.Categories.Get(i));
				if (category)
					m_PVEConfigData.Categories.Insert(category);
			}
		}

		if (m_EditPVEPenalty)
			m_EditPVEPenalty.SetText(m_PVEConfigData.PVEDeathPenaltyPoints.ToString());

		PopulatePVECategoryList();
		if (m_PVEConfigData.Categories.Count() > 0)
			SelectPVECategory(0);
		else
			SelectPVECategory(-1);
	}

	void ApplyPVPConfigData(TrackingModPVPAdminData data)
	{
		int i;
		PVPCategory category;

		if (data)
			TrackingModUI.LogRPC(string.Format("[AdminMenu] ApplyPVPConfigData | Penalty=%1 | Categories=%2", data.PVPDeathPenaltyPoints, data.Categories.Count()));

		m_PVPConfigData = new TrackingModPVPAdminData();
		if (data)
		{
			m_PVPConfigData.PVPDeathPenaltyPoints = data.PVPDeathPenaltyPoints;
			for (i = 0; i < data.Categories.Count(); i++)
			{
				category = ClonePVPCategory(data.Categories.Get(i));
				if (category)
					m_PVPConfigData.Categories.Insert(category);
			}
		}

		if (m_EditPVPPenalty)
			m_EditPVPPenalty.SetText(m_PVPConfigData.PVPDeathPenaltyPoints.ToString());

		PopulatePVPCategoryList();
		if (m_PVPConfigData.Categories.Count() > 0)
			SelectPVPCategory(0);
		else
			SelectPVPCategory(-1);
	}

	void OnAdminConfigSaved(TrackingModAdminSaveResponse response)
	{
		if (!response)
			return;

		if (response.Success)
			SetStatus(response.Message);
		else if (response.Message != "")
			SetStatus(response.Message);
	}

	protected string GetCurrentPlayerID()
	{
		PlayerBase player;
		PlayerIdentity identity;

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return "";

		identity = player.GetIdentity();
		if (!identity)
			return "";

		return identity.GetPlainId();
	}

	protected int GetClampedEditBoxInt(EditBoxWidget editBox, int fallback, int minValue, int maxValue)
	{
		int value;

		value = GetEditBoxInt(editBox, fallback);
		value = Math.Max(minValue, value);
		value = Math.Min(maxValue, value);

		if (editBox)
			editBox.SetText(value.ToString());

		return value;
	}

	TrackingModGeneralAdminData BuildGeneralConfigDataFromWidgets()
	{
		TrackingModGeneralAdminData data;
		int i;

		data = new TrackingModGeneralAdminData();
		if (m_GeneralConfigData && m_GeneralConfigData.AdminIDs)
		{
			for (i = 0; i < m_GeneralConfigData.AdminIDs.Count(); i++)
				data.AdminIDs.Insert(m_GeneralConfigData.AdminIDs.Get(i));
		}
		if (m_GeneralConfigData && m_GeneralConfigData.TrackPVEKillsInZoneTypes)
		{
			for (i = 0; i < m_GeneralConfigData.TrackPVEKillsInZoneTypes.Count(); i++)
				data.TrackPVEKillsInZoneTypes.Insert(m_GeneralConfigData.TrackPVEKillsInZoneTypes.Get(i));
		}

		data.DeletePlayerFilesOlderThanDays = GetClampedEditBoxInt(m_EditDeleteDays, 0, 0, 3650);
		data.UseUTCForDates = GetCheckBoxValue(m_CheckUseUTC);
		data.EnableRewardSystem = GetCheckBoxValue(m_CheckEnableRewardSystem);
		data.DisablePVELeaderboard = !GetCheckBoxValue(m_CheckEnablePVE);
		data.DisablePVPLeaderboard = !GetCheckBoxValue(m_CheckEnablePVP);
		data.MaxPVEPlayersDisplay = GetClampedEditBoxInt(m_EditMaxPVE, 10, 1, 100);
		data.MaxPVPPlayersDisplay = GetClampedEditBoxInt(m_EditMaxPVP, 24, 1, 100);
		data.ShowPlayerOnlineStatusPVE = GetCheckBoxValue(m_CheckShowOnlinePVE);
		data.ShowPlayerOnlineStatusPVP = GetCheckBoxValue(m_CheckShowOnlinePVP);
		data.DisableDeathBySuicide = GetCheckBoxValue(m_CheckDeathSuicide);
		data.DisableDeathByGrenade = GetCheckBoxValue(m_CheckDeathGrenade);
		data.DisableDeathByTrap = GetCheckBoxValue(m_CheckDeathTrap);
		data.DisableDeathByZombie = GetCheckBoxValue(m_CheckDeathZombie);
		data.DisableDeathByAnimal = GetCheckBoxValue(m_CheckDeathAnimal);
		data.DisableDeathByAI = GetCheckBoxValue(m_CheckDeathAI);
		data.DisableDeathByCar = GetCheckBoxValue(m_CheckDeathCar);
		data.DisableDeathByWeapon = GetCheckBoxValue(m_CheckDeathWeapon);
		data.DisableDeathByUnarmed = GetCheckBoxValue(m_CheckDeathUnarmed);
		data.DisableDeathByUnknown = GetCheckBoxValue(m_CheckDeathUnknown);
		data.DisableKillByGrenade = GetCheckBoxValue(m_CheckKillGrenade);
		data.DisableKillByTrap = GetCheckBoxValue(m_CheckKillTrap);
		data.DisableKillByAnimal = GetCheckBoxValue(m_CheckKillAnimal);
		data.DisableKillByZombie = GetCheckBoxValue(m_CheckKillZombie);
		data.DisableKillByCar = GetCheckBoxValue(m_CheckKillCar);
		data.DisableKillByWeapon = GetCheckBoxValue(m_CheckKillWeapon);
		data.DisableKillByUnarmed = GetCheckBoxValue(m_CheckKillUnarmed);
		data.DisableKillByUnknown = GetCheckBoxValue(m_CheckKillUnknown);
		data.DisableKillByUnconsciousSuicide = GetCheckBoxValue(m_CheckKillUnconscious);
		data.ExcludePVPKillWhenBothInPVE = GetCheckBoxValue(m_CheckExcludePVPInPVE);
		data.EnableWebExport = GetCheckBoxValue(m_CheckEnableWebExport);
		data.WebExportIntervalSeconds = GetClampedEditBoxInt(m_EditExportInterval, 300, 1, 86400);
		data.WebExportMaxPlayers = GetClampedEditBoxInt(m_EditExportPlayers, 100, 1, 1000);
		data.WebExportIncludePlayerIDs = GetCheckBoxValue(m_CheckExportPlayerIDs);

		return data;
	}

	protected int GetEditBoxInt(EditBoxWidget editBox, int fallback)
	{
		string text;

		if (!editBox)
			return fallback;

		text = editBox.GetText();
		if (text == "")
			return fallback;

		return text.ToInt();
	}

	protected bool GetCheckBoxValue(CheckBoxWidget checkBox)
	{
		if (!checkBox)
			return false;

		return checkBox.IsChecked();
	}

	protected void PopulatePVECategoryList()
	{
		int i;

		if (!m_PVECategoriesList)
			return;

		m_PVECategoriesList.ClearItems();
		if (!m_PVEConfigData || !m_PVEConfigData.Categories)
			return;

		for (i = 0; i < m_PVEConfigData.Categories.Count(); i++)
			m_PVECategoriesList.AddItem(m_PVEConfigData.Categories.Get(i).CategoryID, null, 0);
	}

	protected void PopulatePVEEntriesList(PVECategory category)
	{
		int i;

		if (!m_PVEEntriesList)
			return;

		m_PVEEntriesList.ClearItems();
		if (!category || !category.ClassNames)
			return;

		for (i = 0; i < category.ClassNames.Count(); i++)
			m_PVEEntriesList.AddItem(category.ClassNames.Get(i), null, 0);
	}

	protected void SelectPVECategory(int index)
	{
		PVECategory category;

		StoreSelectedPVECategoryEdits();
		m_SelectedPVECategoryIndex = index;

		if (!m_PVEConfigData || index < 0 || index >= m_PVEConfigData.Categories.Count())
		{
			m_SelectedPVECategoryIndex = -1;
			if (m_EditPVECategoryId)
				m_EditPVECategoryId.SetText("");
			if (m_EditPVEPreview)
				m_EditPVEPreview.SetText("");
			if (m_EditPVEEntryValue)
				m_EditPVEEntryValue.SetText("");
			PopulatePVEEntriesList(null);
			return;
		}

		category = m_PVEConfigData.Categories.Get(index);
		if (!category)
			return;

		if (m_PVECategoriesList)
			m_PVECategoriesList.SelectRow(index);
		if (m_EditPVECategoryId)
			m_EditPVECategoryId.SetText(category.CategoryID);
		if (m_EditPVEPreview)
			m_EditPVEPreview.SetText(category.ClassNamePreview);
		if (m_EditPVEEntryValue)
			m_EditPVEEntryValue.SetText("");
		PopulatePVEEntriesList(category);
	}

	protected void StoreSelectedPVECategoryEdits()
	{
		PVECategory category;

		if (!m_PVEConfigData || m_SelectedPVECategoryIndex < 0 || m_SelectedPVECategoryIndex >= m_PVEConfigData.Categories.Count())
			return;

		category = m_PVEConfigData.Categories.Get(m_SelectedPVECategoryIndex);
		if (!category)
			return;

		if (m_EditPVECategoryId)
			category.CategoryID = m_EditPVECategoryId.GetText();
		if (m_EditPVEPreview)
			category.ClassNamePreview = m_EditPVEPreview.GetText();

		if (category.CategoryID == "")
			category.CategoryID = "PVECategory" + (m_SelectedPVECategoryIndex + 1).ToString();
	}

	protected TrackingModPVEAdminData BuildPVEConfigDataFromWidgets()
	{
		TrackingModPVEAdminData data;
		int i;
		PVECategory category;

		StoreSelectedPVECategoryEdits();
		data = new TrackingModPVEAdminData();
		data.PVEDeathPenaltyPoints = GetClampedEditBoxInt(m_EditPVEPenalty, 5, 0, 10000);
		for (i = 0; i < m_PVEConfigData.Categories.Count(); i++)
		{
			category = ClonePVECategory(m_PVEConfigData.Categories.Get(i));
			if (category && category.CategoryID != "")
				data.Categories.Insert(category);
		}

		return data;
	}

	protected void PopulatePVPCategoryList()
	{
		int i;

		if (!m_PVPCategoriesList)
			return;

		m_PVPCategoriesList.ClearItems();
		if (!m_PVPConfigData || !m_PVPConfigData.Categories)
			return;

		for (i = 0; i < m_PVPConfigData.Categories.Count(); i++)
			m_PVPCategoriesList.AddItem(m_PVPConfigData.Categories.Get(i).CategoryID, null, 0);
	}

	protected void PopulatePVPEntriesList(PVPCategory category)
	{
		int i;

		if (!m_PVPEntriesList)
			return;

		m_PVPEntriesList.ClearItems();
		if (!category || !category.ClassNames)
			return;

		for (i = 0; i < category.ClassNames.Count(); i++)
			m_PVPEntriesList.AddItem(category.ClassNames.Get(i), null, 0);
	}

	protected void SelectPVPCategory(int index)
	{
		PVPCategory category;

		StoreSelectedPVPCategoryEdits();
		m_SelectedPVPCategoryIndex = index;

		if (!m_PVPConfigData || index < 0 || index >= m_PVPConfigData.Categories.Count())
		{
			m_SelectedPVPCategoryIndex = -1;
			if (m_EditPVPCategoryId)
				m_EditPVPCategoryId.SetText("");
			if (m_EditPVPEntryValue)
				m_EditPVPEntryValue.SetText("");
			PopulatePVPEntriesList(null);
			return;
		}

		category = m_PVPConfigData.Categories.Get(index);
		if (!category)
			return;

		if (m_PVPCategoriesList)
			m_PVPCategoriesList.SelectRow(index);
		if (m_EditPVPCategoryId)
			m_EditPVPCategoryId.SetText(category.CategoryID);
		if (m_EditPVPEntryValue)
			m_EditPVPEntryValue.SetText("");
		PopulatePVPEntriesList(category);
	}

	protected void StoreSelectedPVPCategoryEdits()
	{
		PVPCategory category;

		if (!m_PVPConfigData || m_SelectedPVPCategoryIndex < 0 || m_SelectedPVPCategoryIndex >= m_PVPConfigData.Categories.Count())
			return;

		category = m_PVPConfigData.Categories.Get(m_SelectedPVPCategoryIndex);
		if (!category)
			return;

		if (m_EditPVPCategoryId)
			category.CategoryID = m_EditPVPCategoryId.GetText();

		if (category.CategoryID == "")
			category.CategoryID = "PVPCategory" + (m_SelectedPVPCategoryIndex + 1).ToString();

		category.DisplayName = category.CategoryID;
	}

	protected TrackingModPVPAdminData BuildPVPConfigDataFromWidgets()
	{
		TrackingModPVPAdminData data;
		int i;
		PVPCategory category;

		StoreSelectedPVPCategoryEdits();
		data = new TrackingModPVPAdminData();
		data.PVPDeathPenaltyPoints = GetClampedEditBoxInt(m_EditPVPPenalty, 10, 0, 10000);
		for (i = 0; i < m_PVPConfigData.Categories.Count(); i++)
		{
			category = ClonePVPCategory(m_PVPConfigData.Categories.Get(i));
			if (category && category.CategoryID != "")
				data.Categories.Insert(category);
		}

		return data;
	}

	protected void FillAdminList()
	{
		int i;

		if (!m_AdminIdsList)
			return;

		m_AdminIdsList.ClearItems();
		if (!m_GeneralConfigData || !m_GeneralConfigData.AdminIDs)
			return;

		for (i = 0; i < m_GeneralConfigData.AdminIDs.Count(); i++)
			m_AdminIdsList.AddItem(m_GeneralConfigData.AdminIDs.Get(i), null, 0);
	}

	protected void FillZoneTypeList()
	{
		int i;
		string zoneType;
		string prefix;

		if (!m_ZoneTypesList)
			return;

		m_ZoneTypesList.ClearItems();
		for (i = 0; i < m_ZoneTypeOptions.Count(); i++)
		{
			zoneType = m_ZoneTypeOptions.Get(i);
			prefix = "[ ] ";
			if (m_GeneralConfigData && m_GeneralConfigData.TrackPVEKillsInZoneTypes && m_GeneralConfigData.TrackPVEKillsInZoneTypes.Find(zoneType) != -1)
				prefix = "[x] ";

			m_ZoneTypesList.AddItem(prefix + zoneType, null, 0);
		}
	}

	protected void ToggleSelectedZoneType()
	{
		int selectedRow;
		string zoneType;
		int existingIndex;

		if (!m_ZoneTypesList || !m_GeneralConfigData)
			return;

		selectedRow = m_ZoneTypesList.GetSelectedRow();
		if (selectedRow < 0 || selectedRow >= m_ZoneTypeOptions.Count())
			return;

		zoneType = m_ZoneTypeOptions.Get(selectedRow);
		existingIndex = m_GeneralConfigData.TrackPVEKillsInZoneTypes.Find(zoneType);
		if (existingIndex != -1)
			m_GeneralConfigData.TrackPVEKillsInZoneTypes.Remove(existingIndex);
		else
			m_GeneralConfigData.TrackPVEKillsInZoneTypes.Insert(zoneType);

		FillZoneTypeList();
	}

	protected void CacheWidgets()
	{
		Widget root;

		root = GetLayoutRoot();
		if (!root)
			return;

		m_BtnTabGeneral = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_general"));
		m_BtnTabPVE = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_pve"));
		m_BtnTabPVP = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_pvp"));
		m_BtnTabMilestones = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_milestones"));
		m_BtnTabRewards = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_rewards"));
		m_BtnSaveApply = ButtonWidget.Cast(root.FindAnyWidget("btn_save_apply"));
		m_BtnClose = ButtonWidget.Cast(root.FindAnyWidget("btn_close_admin"));
		m_AdminIdsList = TextListboxWidget.Cast(root.FindAnyWidget("admin_ids_list"));
		m_EditAdminId = EditBoxWidget.Cast(root.FindAnyWidget("edit_admin_id"));
		m_CheckEnablePVE = CheckBoxWidget.Cast(root.FindAnyWidget("check_enable_pve"));
		m_CheckEnablePVP = CheckBoxWidget.Cast(root.FindAnyWidget("check_enable_pvp"));
		m_EditMaxPVE = EditBoxWidget.Cast(root.FindAnyWidget("edit_max_pve"));
		m_EditMaxPVP = EditBoxWidget.Cast(root.FindAnyWidget("edit_max_pvp"));
		m_CheckShowOnlinePVE = CheckBoxWidget.Cast(root.FindAnyWidget("check_show_online_pve"));
		m_CheckShowOnlinePVP = CheckBoxWidget.Cast(root.FindAnyWidget("check_show_online_pvp"));
		m_EditDeleteDays = EditBoxWidget.Cast(root.FindAnyWidget("edit_delete_days"));
		m_CheckUseUTC = CheckBoxWidget.Cast(root.FindAnyWidget("check_use_utc"));
		m_CheckEnableRewardSystem = CheckBoxWidget.Cast(root.FindAnyWidget("check_enable_rewardsystem"));
		m_CheckDeathSuicide = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_suicide"));
		m_CheckDeathGrenade = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_grenade"));
		m_CheckDeathTrap = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_trap"));
		m_CheckDeathZombie = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_zombie"));
		m_CheckDeathAnimal = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_animal"));
		m_CheckDeathAI = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_ai"));
		m_CheckDeathCar = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_car"));
		m_CheckDeathWeapon = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_weapon"));
		m_CheckDeathUnarmed = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_unarmed"));
		m_CheckDeathUnknown = CheckBoxWidget.Cast(root.FindAnyWidget("check_death_unknown"));
		m_CheckKillGrenade = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_grenade"));
		m_CheckKillTrap = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_trap"));
		m_CheckKillAnimal = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_animal"));
		m_CheckKillZombie = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_zombie"));
		m_CheckKillCar = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_car"));
		m_CheckKillWeapon = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_weapon"));
		m_CheckKillUnarmed = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_unarmed"));
		m_CheckKillUnknown = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_unknown"));
		m_CheckKillUnconscious = CheckBoxWidget.Cast(root.FindAnyWidget("check_kill_unconscious"));
		m_CheckExcludePVPInPVE = CheckBoxWidget.Cast(root.FindAnyWidget("check_exclude_pvp_in_pve"));
		m_ZoneTypesList = TextListboxWidget.Cast(root.FindAnyWidget("zone_types_list"));
		m_CheckEnableWebExport = CheckBoxWidget.Cast(root.FindAnyWidget("check_enable_webexport"));
		m_EditExportInterval = EditBoxWidget.Cast(root.FindAnyWidget("edit_export_interval"));
		m_EditExportPlayers = EditBoxWidget.Cast(root.FindAnyWidget("edit_export_players"));
		m_CheckExportPlayerIDs = CheckBoxWidget.Cast(root.FindAnyWidget("check_show_playerids_export"));
		m_EditPVEPenalty = EditBoxWidget.Cast(root.FindAnyWidget("edit_pve_penalty"));
		m_PVECategoriesList = TextListboxWidget.Cast(root.FindAnyWidget("pve_categories_list"));
		m_EditPVECategoryId = EditBoxWidget.Cast(root.FindAnyWidget("edit_pve_category_id"));
		m_EditPVEPreview = EditBoxWidget.Cast(root.FindAnyWidget("edit_pve_preview"));
		m_EditPVEEntryValue = EditBoxWidget.Cast(root.FindAnyWidget("edit_pve_entry_value"));
		m_PVEEntriesList = TextListboxWidget.Cast(root.FindAnyWidget("pve_entries_list"));
		m_EditPVPPenalty = EditBoxWidget.Cast(root.FindAnyWidget("edit_pvp_penalty"));
		m_PVPCategoriesList = TextListboxWidget.Cast(root.FindAnyWidget("pvp_categories_list"));
		m_EditPVPCategoryId = EditBoxWidget.Cast(root.FindAnyWidget("edit_pvp_category_id"));
		m_EditPVPEntryValue = EditBoxWidget.Cast(root.FindAnyWidget("edit_pvp_entry_value"));
		m_PVPEntriesList = TextListboxWidget.Cast(root.FindAnyWidget("pvp_entries_list"));

		m_TabGeneralPanel = root.FindAnyWidget("tab_general_panel");
		m_TabPVEPanel = root.FindAnyWidget("tab_pve_panel");
		m_TabPVPPanel = root.FindAnyWidget("tab_pvp_panel");
		m_TabMilestonesPanel = root.FindAnyWidget("tab_milestones_panel");
		m_TabRewardsPanel = root.FindAnyWidget("tab_rewards_panel");

		m_BtnTabPlayers = ButtonWidget.Cast(root.FindAnyWidget("btn_tab_players"));
		m_TabPlayersPanel = root.FindAnyWidget("tab_players_panel");
		m_EditPlayerSearch = EditBoxWidget.Cast(root.FindAnyWidget("edit_player_search"));
		m_PlayerResultList = TextListboxWidget.Cast(root.FindAnyWidget("player_result_list"));
		m_PlayerSelectedLabel = TextWidget.Cast(root.FindAnyWidget("player_selected_label"));
		m_EditAwardPoints = EditBoxWidget.Cast(root.FindAnyWidget("edit_award_points"));
		m_EditAwardItemClass = EditBoxWidget.Cast(root.FindAnyWidget("edit_award_item_class"));
		m_EditAwardItemAmount = EditBoxWidget.Cast(root.FindAnyWidget("edit_award_item_amount"));
		m_PlayerActionStatus = TextWidget.Cast(root.FindAnyWidget("player_action_status"));
	}

	protected void SetActiveTab(int tabIndex)
	{
		m_ActiveTab = tabIndex;

		if (m_TabGeneralPanel)
			m_TabGeneralPanel.Show(tabIndex == 0);
		if (m_TabPVEPanel)
			m_TabPVEPanel.Show(tabIndex == 1);
		if (m_TabPVPPanel)
			m_TabPVPPanel.Show(tabIndex == 2);
		if (m_TabMilestonesPanel)
			m_TabMilestonesPanel.Show(tabIndex == 3);
		if (m_TabRewardsPanel)
			m_TabRewardsPanel.Show(tabIndex == 4);
		if (m_TabPlayersPanel)
			m_TabPlayersPanel.Show(tabIndex == 5);

		UpdateTabButtonState(m_BtnTabGeneral, tabIndex == 0);
		UpdateTabButtonState(m_BtnTabPVE, tabIndex == 1);
		UpdateTabButtonState(m_BtnTabPVP, tabIndex == 2);
		UpdateTabButtonState(m_BtnTabMilestones, tabIndex == 3);
		UpdateTabButtonState(m_BtnTabRewards, tabIndex == 4);
		UpdateTabButtonState(m_BtnTabPlayers, tabIndex == 5);

		UpdateHeaderState();
	}

	protected void UpdateTabButtonState(ButtonWidget button, bool isActive)
	{
		if (!button)
			return;

		if (isActive)
			button.SetColor(ARGB(255, 255, 140, 30));
		else
			button.SetColor(ARGB(255, 77, 77, 77));
	}

	protected void UpdateHeaderState()
	{
		string tabLabel;

		if (!m_MenuController)
			return;

		switch (m_ActiveTab)
		{
			case 0:
				tabLabel = "Allgemein";
				break;
			case 1:
				tabLabel = "PvE";
				break;
			case 2:
				tabLabel = "PvP";
				break;
			case 3:
				tabLabel = "Milestones";
				break;
			case 4:
				tabLabel = "Rewards";
				break;
			case 5:
				tabLabel = "Players";
				break;
		}

		m_MenuController.CurrentTabLabel = tabLabel;
		m_MenuController.NotifyPropertiesChanged({"CurrentTabLabel"});
	}

	protected void SetStatus(string text)
	{
		if (!m_MenuController)
			return;

		m_MenuController.StatusText = text;
		m_MenuController.NotifyPropertiesChanged({"StatusText"});
	}

	void OnClickTabGeneral()
	{
		SetActiveTab(0);
	}

	void OnClickTabPvE()
	{
		SetActiveTab(1);
	}

	void OnClickTabPvP()
	{
		SetActiveTab(2);
	}

	void OnClickTabMilestones()
	{
		SetActiveTab(3);
	}

	void OnClickTabRewards()
	{
		SetActiveTab(4);
	}

	void OnClickSaveApply()
	{
		TrackingModGeneralAdminData generalData;
		TrackingModPVEAdminData pveData;
		TrackingModPVPAdminData pvpData;
		PlayerBase player;

		if (!m_LeaderboardData || !m_LeaderboardData.isAdmin)
			return;

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		generalData = BuildGeneralConfigDataFromWidgets();
		pveData = BuildPVEConfigDataFromWidgets();
		pvpData = BuildPVPConfigDataFromWidgets();
		SetStatus("Speichere Admin-Konfiguration...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "SaveAdminFullConfig", new Param3<TrackingModGeneralAdminData, TrackingModPVEAdminData, TrackingModPVPAdminData>(generalData, pveData, pvpData), true, player.GetIdentity());
	}

	void OnClickAddPVECategory()
	{
		PVECategory category;

		StoreSelectedPVECategoryEdits();
		category = new PVECategory();
		category.CategoryID = "PVECategory" + (m_PVEConfigData.Categories.Count() + 1).ToString();
		m_PVEConfigData.Categories.Insert(category);
		PopulatePVECategoryList();
		SelectPVECategory(m_PVEConfigData.Categories.Count() - 1);
	}

	void OnClickDeletePVECategory()
	{
		if (!m_PVEConfigData || m_SelectedPVECategoryIndex < 0 || m_SelectedPVECategoryIndex >= m_PVEConfigData.Categories.Count())
			return;

		m_PVEConfigData.Categories.Remove(m_SelectedPVECategoryIndex);
		PopulatePVECategoryList();
		if (m_PVEConfigData.Categories.Count() > 0)
			SelectPVECategory(Math.Min(m_SelectedPVECategoryIndex, m_PVEConfigData.Categories.Count() - 1));
		else
			SelectPVECategory(-1);
	}

	void OnClickAddPVEEntry()
	{
		PVECategory category;
		string entryValue;

		if (!m_PVEConfigData || m_SelectedPVECategoryIndex < 0 || m_SelectedPVECategoryIndex >= m_PVEConfigData.Categories.Count())
			return;

		entryValue = "";
		if (m_EditPVEEntryValue)
			entryValue = m_EditPVEEntryValue.GetText();
		if (entryValue == "")
			return;

		category = m_PVEConfigData.Categories.Get(m_SelectedPVECategoryIndex);
		if (!category)
			return;

		category.ClassNames.Insert(entryValue);
		if (m_EditPVEEntryValue)
			m_EditPVEEntryValue.SetText("");
		PopulatePVEEntriesList(category);
	}

	void OnClickDeletePVEEntry()
	{
		PVECategory category;
		int selectedRow;

		if (!m_PVEConfigData || !m_PVEEntriesList || m_SelectedPVECategoryIndex < 0 || m_SelectedPVECategoryIndex >= m_PVEConfigData.Categories.Count())
			return;

		selectedRow = m_PVEEntriesList.GetSelectedRow();
		if (selectedRow < 0)
			return;

		category = m_PVEConfigData.Categories.Get(m_SelectedPVECategoryIndex);
		if (!category || selectedRow >= category.ClassNames.Count())
			return;

		category.ClassNames.Remove(selectedRow);
		PopulatePVEEntriesList(category);
	}

	void OnClickAddPVPCategory()
	{
		PVPCategory category;

		StoreSelectedPVPCategoryEdits();
		category = new PVPCategory();
		category.CategoryID = "PVPCategory" + (m_PVPConfigData.Categories.Count() + 1).ToString();
		category.DisplayName = category.CategoryID;
		m_PVPConfigData.Categories.Insert(category);
		PopulatePVPCategoryList();
		SelectPVPCategory(m_PVPConfigData.Categories.Count() - 1);
	}

	void OnClickDeletePVPCategory()
	{
		if (!m_PVPConfigData || m_SelectedPVPCategoryIndex < 0 || m_SelectedPVPCategoryIndex >= m_PVPConfigData.Categories.Count())
			return;

		m_PVPConfigData.Categories.Remove(m_SelectedPVPCategoryIndex);
		PopulatePVPCategoryList();
		if (m_PVPConfigData.Categories.Count() > 0)
			SelectPVPCategory(Math.Min(m_SelectedPVPCategoryIndex, m_PVPConfigData.Categories.Count() - 1));
		else
			SelectPVPCategory(-1);
	}

	void OnClickAddPVPEntry()
	{
		PVPCategory category;
		string entryValue;

		if (!m_PVPConfigData || m_SelectedPVPCategoryIndex < 0 || m_SelectedPVPCategoryIndex >= m_PVPConfigData.Categories.Count())
			return;

		entryValue = "";
		if (m_EditPVPEntryValue)
			entryValue = m_EditPVPEntryValue.GetText();
		if (entryValue == "")
			return;

		category = m_PVPConfigData.Categories.Get(m_SelectedPVPCategoryIndex);
		if (!category)
			return;

		category.ClassNames.Insert(entryValue);
		if (m_EditPVPEntryValue)
			m_EditPVPEntryValue.SetText("");
		PopulatePVPEntriesList(category);
	}

	void OnClickDeletePVPEntry()
	{
		PVPCategory category;
		int selectedRow;

		if (!m_PVPConfigData || !m_PVPEntriesList || m_SelectedPVPCategoryIndex < 0 || m_SelectedPVPCategoryIndex >= m_PVPConfigData.Categories.Count())
			return;

		selectedRow = m_PVPEntriesList.GetSelectedRow();
		if (selectedRow < 0)
			return;

		category = m_PVPConfigData.Categories.Get(m_SelectedPVPCategoryIndex);
		if (!category || selectedRow >= category.ClassNames.Count())
			return;

		category.ClassNames.Remove(selectedRow);
		PopulatePVPEntriesList(category);
	}

	void OnClickAddAdminId()
	{
		string adminID;

		if (!m_EditAdminId || !m_GeneralConfigData)
			return;

		adminID = m_EditAdminId.GetText();
		if (adminID == "")
			return;
		if (m_GeneralConfigData.AdminIDs.Find(adminID) != -1)
			return;

		m_GeneralConfigData.AdminIDs.Insert(adminID);
		m_EditAdminId.SetText("");
		FillAdminList();
	}

	void OnClickRemoveAdminId()
	{
		int selectedRow;
		string adminID;
		string currentPlayerID;

		if (!m_AdminIdsList || !m_GeneralConfigData)
			return;

		selectedRow = m_AdminIdsList.GetSelectedRow();
		if (selectedRow < 0 || selectedRow >= m_GeneralConfigData.AdminIDs.Count())
			return;

		adminID = m_GeneralConfigData.AdminIDs.Get(selectedRow);
		currentPlayerID = GetCurrentPlayerID();
		if (adminID == currentPlayerID)
		{
			SetStatus("Eigene Admin-ID kann nicht entfernt werden");
			return;
		}

		selectedRow = m_GeneralConfigData.AdminIDs.Find(adminID);
		if (selectedRow != -1)
			m_GeneralConfigData.AdminIDs.Remove(selectedRow);

		FillAdminList();
	}

	void SelectPlayerFromList(int row)
	{
		string rowText;
		int openParen;
		int closeParen;

		if (!m_PlayerResultList || row < 0)
			return;

		m_PlayerResultList.GetItemText(row, 0, rowText);
		openParen = rowText.IndexOf("(");
		closeParen = rowText.IndexOf(")");
		if (openParen > 0 && closeParen > openParen)
		{
			m_SelectedPlayerID = rowText.Substring(openParen + 1, closeParen - openParen - 1);
			m_SelectedPlayerName = rowText.Substring(0, openParen - 1);
		}
		UpdateSelectedPlayerLabel();
	}

	void OnClickTabPlayers()
	{
		SetActiveTab(5);
	}

	void OnClickSearchPlayer()
	{
		PlayerBase player;
		string searchTerm;

		if (!m_EditPlayerSearch)
			return;

		searchTerm = m_EditPlayerSearch.GetText();
		if (searchTerm == "")
			return;

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Suche...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminSearchPlayer", new Param1<string>(searchTerm), true, player.GetIdentity());
	}

	void OnClickWipePlayerData()
	{
		PlayerBase player;

		if (m_SelectedPlayerID == "")
		{
			SetPlayerActionStatus("Kein Spieler ausgewaehlt!");
			return;
		}

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Loesche Daten fuer " + m_SelectedPlayerName + "...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminWipePlayerData", new Param1<string>(m_SelectedPlayerID), true, player.GetIdentity());
	}

	void OnClickWipePlayerPoints()
	{
		PlayerBase player;

		if (m_SelectedPlayerID == "")
		{
			SetPlayerActionStatus("Kein Spieler ausgewaehlt!");
			return;
		}

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Setze Points zurueck fuer " + m_SelectedPlayerName + "...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminWipePlayerPoints", new Param1<string>(m_SelectedPlayerID), true, player.GetIdentity());
	}

	void OnClickAwardPVEPoints()
	{
		PlayerBase player;
		int amount;

		if (m_SelectedPlayerID == "")
		{
			SetPlayerActionStatus("Kein Spieler ausgewaehlt!");
			return;
		}

		if (!m_EditAwardPoints)
			return;

		amount = m_EditAwardPoints.GetText().ToInt();
		if (amount <= 0)
		{
			SetPlayerActionStatus("Ungueltige Punktzahl!");
			return;
		}

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Vergebe " + amount.ToString() + " PvE Points an " + m_SelectedPlayerName + "...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminAwardPoints", new Param3<string, int, bool>(m_SelectedPlayerID, amount, true), true, player.GetIdentity());
	}

	void OnClickAwardPVPPoints()
	{
		PlayerBase player;
		int amount;

		if (m_SelectedPlayerID == "")
		{
			SetPlayerActionStatus("Kein Spieler ausgewaehlt!");
			return;
		}

		if (!m_EditAwardPoints)
			return;

		amount = m_EditAwardPoints.GetText().ToInt();
		if (amount <= 0)
		{
			SetPlayerActionStatus("Ungueltige Punktzahl!");
			return;
		}

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Vergebe " + amount.ToString() + " PvP Points an " + m_SelectedPlayerName + "...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminAwardPoints", new Param3<string, int, bool>(m_SelectedPlayerID, amount, false), true, player.GetIdentity());
	}

	void OnClickAwardItem()
	{
		PlayerBase player;
		string className;
		int amount;

		if (m_SelectedPlayerID == "")
		{
			SetPlayerActionStatus("Kein Spieler ausgewaehlt!");
			return;
		}

		if (!m_EditAwardItemClass || !m_EditAwardItemAmount)
			return;

		className = m_EditAwardItemClass.GetText();
		amount = m_EditAwardItemAmount.GetText().ToInt();
		if (className == "")
		{
			SetPlayerActionStatus("Kein Item-Classname angegeben!");
			return;
		}
		if (amount <= 0)
			amount = 1;

		player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		SetPlayerActionStatus("Gebe " + amount.ToString() + "x " + className + " an " + m_SelectedPlayerName + "...");
		GetRPCManager().SendRPC("Ninjins_LeaderBoard", "AdminAwardItem", new Param3<string, string, int>(m_SelectedPlayerID, className, amount), true, player.GetIdentity());
	}

	void ReceiveAdminPlayerSearch(string resultJSON)
	{
		int i;
		int separatorIdx;
		string entry;
		string pID;
		string pName;

		if (!m_PlayerResultList)
			return;

		m_PlayerResultList.ClearItems();
		m_SelectedPlayerID = "";
		m_SelectedPlayerName = "";

		if (resultJSON == "" || resultJSON == "[]")
		{
			SetPlayerActionStatus("Keine Spieler gefunden.");
			UpdateSelectedPlayerLabel();
			return;
		}

		array<string> entries;
		entries = new array<string>();
		resultJSON.Split("|", entries);

		for (i = 0; i < entries.Count(); i++)
		{
			entry = entries.Get(i);
			separatorIdx = entry.IndexOf(":");
			if (separatorIdx > 0)
			{
				pID = entry.Substring(0, separatorIdx);
				pName = entry.Substring(separatorIdx + 1, entry.Length() - separatorIdx - 1);
				m_PlayerResultList.AddItem(pName + " (" + pID + ")", null, 0);
			}
		}

		SetPlayerActionStatus(entries.Count().ToString() + " Spieler gefunden.");
	}

	void ReceiveAdminActionResult(TrackingModAdminSaveResponse response)
	{
		if (!response)
			return;

		if (response.Success)
			SetPlayerActionStatus("Erfolg: " + response.Message);
		else
			SetPlayerActionStatus("Fehler: " + response.Message);
	}

	void SetPlayerActionStatus(string text)
	{
		if (m_PlayerActionStatus)
			m_PlayerActionStatus.SetText(text);
	}

	void UpdateSelectedPlayerLabel()
	{
		if (!m_PlayerSelectedLabel)
			return;

		if (m_SelectedPlayerID != "")
			m_PlayerSelectedLabel.SetText("Ausgewaehlt: " + m_SelectedPlayerName + " (" + m_SelectedPlayerID + ")");
		else
			m_PlayerSelectedLabel.SetText("Kein Spieler ausgewaehlt");
	}

	void OnClickCloseAdmin()
	{
		CloseAdminMenu();
	}

	protected void CloseAdminMenu()
	{
		GetGame().GetMission().GetHud().Show(true);
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
		skip.Insert(UAOpenTrackingModAdminMenu);

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

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_ZoneTypesList)
		{
			ToggleSelectedZoneType();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_PVECategoriesList)
		{
			SelectPVECategory(row);
			return true;
		}

		if (w == m_PVPCategoriesList)
		{
			SelectPVPCategory(row);
			return true;
		}

		if (w == m_PlayerResultList)
		{
			SelectPlayerFromList(row);
			return true;
		}

		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
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
		UAInput input;

		super.Update(dt);
		input = GetUApi().GetInputByName("UAUIBack");
		if (input && input.LocalPress())
		{
			CloseAdminMenu();
		}
	}

	override typename GetControllerType()
	{
		return TrackingModAdminMenuController;
	}

	override string GetLayoutFile()
	{
		return "Ninjins_LeaderBoard\\GUI\\layouts\\Admin\\trackingModAdminMenu.layout";
	}
}

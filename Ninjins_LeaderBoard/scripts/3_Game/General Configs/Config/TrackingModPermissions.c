const string TRACKING_MOD_PERMISSIONS_DIR = TRACKING_MOD_ROOT_FOLDER + "Permissions\\";
const string TRACKING_MOD_ADMINS_FILE = TRACKING_MOD_PERMISSIONS_DIR + "admins.json";

class TrackingModPermissionsFile
{
	ref array<string> AdminSteamIDs;

	void TrackingModPermissionsFile()
	{
		AdminSteamIDs = new array<string>();
	}
}

class TrackingModPermissions
{
	protected static ref TrackingModPermissions s_NJN_Instance;
	protected ref array<string> m_NJN_AdminIDs;

	void TrackingModPermissions()
	{
		m_NJN_AdminIDs = new array<string>();
	}

	static TrackingModPermissions GetInstance()
	{
		if (!s_NJN_Instance)
		{
			s_NJN_Instance = new TrackingModPermissions();
		}
		return s_NJN_Instance;
	}

	void LoadPermissions()
	{
		TrackingModPermissionsFile permFile;
		int i;
		string steamID;

		m_NJN_AdminIDs.Clear();

		if (!g_Game || !g_Game.IsDedicatedServer())
			return;

		if (!FileExist(TRACKING_MOD_PERMISSIONS_DIR))
		{
			CF_Directory.CreateDirectory(TRACKING_MOD_PERMISSIONS_DIR);
			Print("[TrackingMod] Created Permissions directory: " + TRACKING_MOD_PERMISSIONS_DIR);
		}

		if (FileExist(TRACKING_MOD_ADMINS_FILE))
		{
			permFile = new TrackingModPermissionsFile();
			JsonFileLoader<TrackingModPermissionsFile>.JsonLoadFile(TRACKING_MOD_ADMINS_FILE, permFile);
			if (permFile && permFile.AdminSteamIDs)
			{
				for (i = 0; i < permFile.AdminSteamIDs.Count(); i++)
				{
					steamID = permFile.AdminSteamIDs.Get(i);
					if (steamID != "")
					{
						m_NJN_AdminIDs.Insert(steamID);
					}
				}
			}
			Print("[TrackingMod] Loaded " + m_NJN_AdminIDs.Count().ToString() + " admin(s) from permissions file");
		}
		else
		{
			permFile = new TrackingModPermissionsFile();
			JsonFileLoader<TrackingModPermissionsFile>.JsonSaveFile(TRACKING_MOD_ADMINS_FILE, permFile);
			Print("[TrackingMod] Created default empty admins.json at: " + TRACKING_MOD_ADMINS_FILE);
		}
	}

	bool IsAdmin(string playerID)
	{
		int i;

		if (!m_NJN_AdminIDs || m_NJN_AdminIDs.Count() == 0)
			return false;

		for (i = 0; i < m_NJN_AdminIDs.Count(); i++)
		{
			if (m_NJN_AdminIDs.Get(i) == playerID)
				return true;
		}
		return false;
	}

	array<string> GetAllAdminIDs()
	{
		return m_NJN_AdminIDs;
	}
}

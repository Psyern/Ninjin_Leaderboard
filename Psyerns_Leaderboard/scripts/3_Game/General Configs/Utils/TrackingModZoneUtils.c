#ifdef NinjinsPvPPvE
class TrackingModZoneUtils
{
	private static ref map<string, bool> m_PlayerInPVPZone;
	private static ref map<string, bool> m_PlayerInPVEZone;
	private static ref map<string, bool> m_PlayerInVisualZone;
	private static ref map<string, bool> m_PlayerInRaidZone;
	private static ref map<string, string> m_PlayerCurrentZoneName;
	static void Init()
	{
		if (!m_PlayerInPVPZone)
			m_PlayerInPVPZone = new map<string, bool>();
		if (!m_PlayerInPVEZone)
			m_PlayerInPVEZone = new map<string, bool>();
		if (!m_PlayerInVisualZone)
			m_PlayerInVisualZone = new map<string, bool>();
		if (!m_PlayerInRaidZone)
			m_PlayerInRaidZone = new map<string, bool>();
		if (!m_PlayerCurrentZoneName)
			m_PlayerCurrentZoneName = new map<string, string>();
	}
	static void SetPlayerInPVPZone(string playerID, bool inZone)
	{
		Init();
		m_PlayerInPVPZone.Set(playerID, inZone);
	}
	static void SetPlayerInPVEZone(string playerID, bool inZone)
	{
		Init();
		m_PlayerInPVEZone.Set(playerID, inZone);
	}
	static void SetPlayerInVisualZone(string playerID, bool inZone)
	{
		Init();
		m_PlayerInVisualZone.Set(playerID, inZone);
	}
	static void SetPlayerInRaidZone(string playerID, bool inZone)
	{
		Init();
		m_PlayerInRaidZone.Set(playerID, inZone);
	}
	static void SetPlayerZoneName(string playerID, string zoneName)
	{
		Init();
		if (zoneName != "")
			m_PlayerCurrentZoneName.Set(playerID, zoneName);
		else
			m_PlayerCurrentZoneName.Remove(playerID);
	}
	static bool IsPlayerInPVPZone(string playerID)
	{
		if (!m_PlayerInPVPZone)
			return false;
		if (m_PlayerInPVPZone.Contains(playerID))
			return m_PlayerInPVPZone.Get(playerID);
		return false;
	}
	static bool IsPlayerInPVEZone(string playerID)
	{
		if (!m_PlayerInPVEZone)
			return false;
		if (m_PlayerInPVEZone.Contains(playerID))
			return m_PlayerInPVEZone.Get(playerID);
		return false;
	}
	static bool IsPlayerInVisualZone(string playerID)
	{
		if (!m_PlayerInVisualZone)
			return false;
		if (m_PlayerInVisualZone.Contains(playerID))
			return m_PlayerInVisualZone.Get(playerID);
		return false;
	}
	static bool IsPlayerInRaidZone(string playerID)
	{
		if (!m_PlayerInRaidZone)
			return false;
		if (m_PlayerInRaidZone.Contains(playerID))
			return m_PlayerInRaidZone.Get(playerID);
		return false;
	}
	static string GetPlayerZoneName(string playerID)
	{
		if (!m_PlayerCurrentZoneName)
			return "";
		if (m_PlayerCurrentZoneName.Contains(playerID))
			return m_PlayerCurrentZoneName.Get(playerID);
		return "";
	}
	static bool MatchesZoneName(string zoneName, string pattern)
	{
		if (pattern == "")
			return false;
		if (pattern == zoneName)
			return true;
		if (pattern.Length() > 0 && pattern.Get(pattern.Length() - 1) == "_")
		{
			string prefix = pattern.Substring(0, pattern.Length() - 1);
			if (zoneName.Length() >= prefix.Length())
			{
				string zonePrefix = zoneName.Substring(0, prefix.Length());
				if (zonePrefix == prefix)
					return true;
			}
		}
		return false;
	}
}
#endif
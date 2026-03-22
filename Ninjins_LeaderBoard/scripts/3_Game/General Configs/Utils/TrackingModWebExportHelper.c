#ifdef PSYERNS_FRAMEWORK
class PF_LeaderboardApi : PF_WebApiBase
{
	protected string m_ApiKey;

	void PF_LeaderboardApi(string baseUrl, string apiKey)
	{
		m_BaseUrl = baseUrl;
		m_ApiKey = apiKey;
		m_RestContext = m_Rest.GetRestContext(m_BaseUrl);
		m_RestContext.SetHeader("application/json");
	}

	void SendLeaderboardData(string jsonData)
	{
		PF_HttpArguments args = new PF_HttpArguments();
		args.Add("api_key", m_ApiKey);
		string endpoint = args.ToQuery("/receive");

		PF_Logger.Log("Sending leaderboard data to endpoint...");
		Post(endpoint, jsonData);
	}
}

class TrackingModWebExportHelper
{
	protected static ref PF_LeaderboardApi s_Api;
	protected static bool s_Initialized;

	static void Init()
	{
		s_Initialized = false;
		s_Api = null;

		PF_WebConfig config = PF_WebConfig.GetInstance();
		if (!config)
		{
			PF_Logger.Error("TrackingModWebExportHelper::Init - WebConfig not available");
			return;
		}

		PF_WebEndpoint ep = config.GetEndpoint("Leaderboard");
		if (!ep)
		{
			PF_Logger.Debug("TrackingModWebExportHelper::Init - No 'Leaderboard' endpoint configured");
			return;
		}

		if (!ep.Enabled)
		{
			PF_Logger.Debug("TrackingModWebExportHelper::Init - Leaderboard endpoint is disabled");
			return;
		}

		if (ep.BaseUrl == "" || ep.ApiKey == "")
		{
			PF_Logger.Error("TrackingModWebExportHelper::Init - Leaderboard endpoint missing BaseUrl or ApiKey");
			return;
		}

		s_Api = new PF_LeaderboardApi(ep.BaseUrl, ep.ApiKey);
		s_Initialized = true;
		PF_Logger.Log("TrackingModWebExportHelper initialized - endpoint: " + ep.BaseUrl);
	}

	static bool IsInitialized()
	{
		return s_Initialized;
	}

	static void SendExport()
	{
		if (!s_Initialized || !s_Api)
		{
			PF_Logger.Debug("TrackingModWebExportHelper::SendExport - not initialized, skipping");
			return;
		}

		if (!FileExist(TRACKING_MOD_WEB_EXPORT_FILE))
		{
			PF_Logger.Debug("TrackingModWebExportHelper::SendExport - export file not found: " + TRACKING_MOD_WEB_EXPORT_FILE);
			return;
		}

		FileHandle file = OpenFile(TRACKING_MOD_WEB_EXPORT_FILE, FileMode.READ);
		if (!file)
		{
			PF_Logger.Error("TrackingModWebExportHelper::SendExport - failed to open file: " + TRACKING_MOD_WEB_EXPORT_FILE);
			return;
		}

		string jsonData = "";
		string line;
		while (FGets(file, line) >= 0)
		{
			jsonData += line;
		}
		CloseFile(file);

		if (jsonData == "")
		{
			PF_Logger.Debug("TrackingModWebExportHelper::SendExport - export file is empty");
			return;
		}

		PF_Logger.Log("TrackingModWebExportHelper::SendExport - sending " + jsonData.Length().ToString() + " bytes");
		s_Api.SendLeaderboardData(jsonData);
	}
}
#endif

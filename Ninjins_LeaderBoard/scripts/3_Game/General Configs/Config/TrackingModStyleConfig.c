const string TRACKING_MOD_STYLE_CONFIG_FILE = TRACKING_MOD_DATA_DIR + "StyleConfig.json";
ref TrackingModStyleConfig g_TrackingModStyleConfig;

class TrackingModColorSetting
{
	int R;
	int G;
	int B;
	int A;

	void TrackingModColorSetting()
	{
		R = 255;
		G = 255;
		B = 255;
		A = 255;
	}

	int ToARGB()
	{
		return ARGB(A, R, G, B);
	}
}

class TrackingModStyleConfig
{
	ref TrackingModColorSetting HeaderBackgroundColor;
	ref TrackingModColorSetting HeaderTextColor;
	int HeaderFontSize;
	ref TrackingModColorSetting RowEvenColor;
	ref TrackingModColorSetting RowOddColor;
	ref TrackingModColorSetting RowTextColor;
	int RowFontSize;
	ref TrackingModColorSetting CurrentPlayerHighlightColor;
	ref TrackingModColorSetting OnlineIndicatorColor;
	ref TrackingModColorSetting OfflineIndicatorColor;
	ref TrackingModColorSetting BackgroundColor;
	ref TrackingModColorSetting BorderColor;
	float BackgroundOpacity;
	string FontName;
	ref TrackingModColorSetting TitleColor;
	int TitleFontSize;
	string LeaderboardTitle;

	void TrackingModStyleConfig()
	{
		HeaderBackgroundColor = new TrackingModColorSetting();
		HeaderBackgroundColor.R = 30;
		HeaderBackgroundColor.G = 30;
		HeaderBackgroundColor.B = 30;
		HeaderBackgroundColor.A = 240;

		HeaderTextColor = new TrackingModColorSetting();
		HeaderTextColor.R = 255;
		HeaderTextColor.G = 200;
		HeaderTextColor.B = 100;
		HeaderTextColor.A = 255;
		HeaderFontSize = 14;

		RowEvenColor = new TrackingModColorSetting();
		RowEvenColor.R = 20;
		RowEvenColor.G = 20;
		RowEvenColor.B = 20;
		RowEvenColor.A = 200;

		RowOddColor = new TrackingModColorSetting();
		RowOddColor.R = 30;
		RowOddColor.G = 30;
		RowOddColor.B = 30;
		RowOddColor.A = 200;

		RowTextColor = new TrackingModColorSetting();
		RowTextColor.R = 220;
		RowTextColor.G = 220;
		RowTextColor.B = 220;
		RowTextColor.A = 255;
		RowFontSize = 13;

		CurrentPlayerHighlightColor = new TrackingModColorSetting();
		CurrentPlayerHighlightColor.R = 255;
		CurrentPlayerHighlightColor.G = 215;
		CurrentPlayerHighlightColor.B = 0;
		CurrentPlayerHighlightColor.A = 80;

		OnlineIndicatorColor = new TrackingModColorSetting();
		OnlineIndicatorColor.R = 0;
		OnlineIndicatorColor.G = 200;
		OnlineIndicatorColor.B = 0;
		OnlineIndicatorColor.A = 255;

		OfflineIndicatorColor = new TrackingModColorSetting();
		OfflineIndicatorColor.R = 200;
		OfflineIndicatorColor.G = 0;
		OfflineIndicatorColor.B = 0;
		OfflineIndicatorColor.A = 255;

		BackgroundColor = new TrackingModColorSetting();
		BackgroundColor.R = 15;
		BackgroundColor.G = 15;
		BackgroundColor.B = 15;
		BackgroundColor.A = 245;

		BorderColor = new TrackingModColorSetting();
		BorderColor.R = 255;
		BorderColor.G = 140;
		BorderColor.B = 30;
		BorderColor.A = 255;

		BackgroundOpacity = 0.95;
		FontName = "gui/fonts/metronbook14";

		TitleColor = new TrackingModColorSetting();
		TitleColor.R = 255;
		TitleColor.G = 140;
		TitleColor.B = 30;
		TitleColor.A = 255;
		TitleFontSize = 18;
		LeaderboardTitle = "Ninjins Leaderboard";
	}

	void SaveConfig()
	{
		if (!g_Game || !g_Game.IsDedicatedServer())
			return;
		TrackingModData.CheckDirectories();
		JsonFileLoader<TrackingModStyleConfig>.JsonSaveFile(TRACKING_MOD_STYLE_CONFIG_FILE, this);
	}

	static TrackingModStyleConfig LoadConfig()
	{
		TrackingModStyleConfig config;

		if (!g_Game || !g_Game.IsDedicatedServer())
			return null;

		TrackingModData.CheckDirectories();
		if (FileExist(TRACKING_MOD_STYLE_CONFIG_FILE))
		{
			config = new TrackingModStyleConfig();
			JsonFileLoader<TrackingModStyleConfig>.JsonLoadFile(TRACKING_MOD_STYLE_CONFIG_FILE, config);
			Print("[TrackingMod] StyleConfig loaded from: " + TRACKING_MOD_STYLE_CONFIG_FILE);
		}
		else
		{
			config = new TrackingModStyleConfig();
			config.SaveConfig();
			Print("[TrackingMod] StyleConfig.json not found. Created default.");
		}
		return config;
	}
}

class TrackingModMilestoneRewardConfig
{
	ref array<ref TrackingModRewardSection> RewardItems;
	ref array<ref TrackingModCurrencyReward> CurrencyRewards;
	int CurrencyMin;
	int CurrencyMax;
	
	void TrackingModMilestoneRewardConfig()
	{
		RewardItems = new array<ref TrackingModRewardSection>;
		CurrencyRewards = new array<ref TrackingModCurrencyReward>;
		CurrencyMin = 1;
		CurrencyMax = 2;
	}
}

class TrackingModMilestoneRewardLoader
{
	static TrackingModMilestoneRewardConfig LoadRewardConfig(string fileName)
	{
		string filePath;
		string actualFilePath;
		TrackingModMilestoneRewardConfig config;
		array<ref CF_File> files;
		CF_File foundFile;
		int i;
		string fileFullPath;
		
		if (!GetGame().IsDedicatedServer())
			return null;
		
		if (!g_TrackingModRewardConfig)
			return null;
		
		actualFilePath = g_TrackingModRewardConfig.RewardConfigFolder;
		if (actualFilePath == "")
			actualFilePath = TRACKING_MOD_REWARD_SYSTEM_DIR;
		
		if (!FileExist(actualFilePath))
		{
			CF_Directory.CreateDirectory(actualFilePath);
			TrackingMod.LogInfo("Created RewardSystem directory: " + actualFilePath);
		}
		
		filePath = actualFilePath + fileName;
		if (FileExist(filePath))
		{
			config = new TrackingModMilestoneRewardConfig();
			JsonFileLoader<TrackingModMilestoneRewardConfig>.JsonLoadFile(filePath, config);
			TrackingMod.LogInfo("[TrackingModMilestoneRewardLoader] Loaded reward config from: " + filePath);
			return config;
		}
		
		files = new array<ref CF_File>;
		if (CF_Directory.GetFiles(actualFilePath + "*", files, FindFileFlags.ALL))
		{
			for (i = 0; i < files.Count(); i++)
			{
				foundFile = files.Get(i);
				if (foundFile)
				{
					fileFullPath = foundFile.GetFullPath();
					if (CF_Path.GetFileName(fileFullPath) == fileName)
					{
						config = new TrackingModMilestoneRewardConfig();
						JsonFileLoader<TrackingModMilestoneRewardConfig>.JsonLoadFile(fileFullPath, config);
						TrackingMod.LogInfo("[TrackingModMilestoneRewardLoader] Loaded reward config from: " + fileFullPath);
						return config;
					}
				}
			}
		}
		
		TrackingMod.LogWarning("[TrackingModMilestoneRewardLoader] Reward config file not found: " + fileName + " in " + actualFilePath);
		return null;
	}
}

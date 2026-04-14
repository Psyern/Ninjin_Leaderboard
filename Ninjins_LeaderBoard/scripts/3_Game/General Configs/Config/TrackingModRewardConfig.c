const string TRACKING_MOD_REWARD_CONFIG_FILE = TRACKING_MOD_DATA_DIR + "TrackingModRewardConfig.json";
const string TRACKING_MOD_REWARD_SYSTEM_DIR = TRACKING_MOD_DATA_DIR + "RewardSystem\\";
ref TrackingModRewardConfig g_TrackingModRewardConfig;

class RewardConfigEntry
{
	string Milestone;
	string FileName;
	
	void RewardConfigEntry()
	{
		Milestone = "";
		FileName = "";
	}
}

class TrackingModRewardAttachment
{
	string ItemClassName;
	float SpawnChance;
	int Amount;
	float QuantMin;
	float QuantMax;
	float HealthMin;
	float HealthMax;
	ref array<ref TrackingModRewardAttachment> Attachments;
	
	void TrackingModRewardAttachment()
	{
		ItemClassName = "";
		SpawnChance = 100.0;
		Amount = 0;
		QuantMin = 0.0;
		QuantMax = 0.0;
		HealthMin = 0.0;
		HealthMax = 0.0;
		Attachments = new array<ref TrackingModRewardAttachment>;
	}
}

class TrackingModRewardItem
{
	string ItemClassName;
	float SpawnChance;
	int Amount;
	float QuantMin;
	float QuantMax;
	float HealthMin;
	float HealthMax;
	ref array<ref TrackingModRewardAttachment> Attachments;
	
	void TrackingModRewardItem()
	{
		ItemClassName = "";
		SpawnChance = 100.0;
		Amount = 0;
		QuantMin = 0.0;
		QuantMax = 0.0;
		HealthMin = 0.0;
		HealthMax = 0.0;
		Attachments = new array<ref TrackingModRewardAttachment>;
	}
}

class TrackingModCurrencyReward
{
	string ClassName;
	float SpawnChance;
	int Amount;
	
	void TrackingModCurrencyReward()
	{
		ClassName = "";
		SpawnChance = 100.0;
		Amount = 0;
	}
}

class TrackingModSimpleCashReward
{
	bool Enabled;
	string CurrencyClassName;
	int ZombieKillInterval;
	int ZombieRewardAmount;
	int AnimalKillInterval;
	int AnimalRewardAmount;
	int AIKillInterval;
	int AIRewardAmount;
	int PlayerKillInterval;
	int PlayerRewardAmount;

	void TrackingModSimpleCashReward()
	{
		Enabled = false;
		CurrencyClassName = "Hryvnia";
		ZombieKillInterval = 10;
		ZombieRewardAmount = 50;
		AnimalKillInterval = 5;
		AnimalRewardAmount = 100;
		AIKillInterval = 5;
		AIRewardAmount = 150;
		PlayerKillInterval = 3;
		PlayerRewardAmount = 200;
	}
}

class TrackingModRewardSection
{
	string Name;
	float SpawnChance;
	int ItemsMin;
	int ItemsMax;
	ref array<ref TrackingModRewardItem> LootItems;
	
	void TrackingModRewardSection()
	{
		Name = "";
		SpawnChance = 100.0;
		ItemsMin = 1;
		ItemsMax = 1;
		LootItems = new array<ref TrackingModRewardItem>;
	}
}

class TrackingModRewardConfig
{
	bool EnableMilestoneRewards;
	string RewardConfigFolder;
	ref map<string, ref array<int>> PVEMilestones;
	ref map<string, ref array<int>> PVPMilestones;
	ref map<string, ref array<ref RewardConfigEntry>> PVERewardConfigs;
	ref map<string, ref array<ref RewardConfigEntry>> PVPRewardConfigs;
	ref TrackingModSimpleCashReward SimpleCashRewards;
	bool EnableServerwideMilestoneNotification;
	string ServerwideMilestoneMessage;
	bool EnableCrateSpawnOnMilestone;
	float CrateSpawnRadius;
	string CrateContainerClassName;
	bool EnableCrateProximityNotification;
	float CrateProximityCheckInterval;
	float CrateProximityNotifyRadius;
	float CrateDespawnTime;

	void TrackingModRewardConfig()
	{
		EnableMilestoneRewards = true;
		RewardConfigFolder = "$profile:Ninjins_Tracking_Mod\\Data\\RewardSystem\\";
		PVEMilestones = new map<string, ref array<int>>;
		PVPMilestones = new map<string, ref array<int>>;
		PVERewardConfigs = new map<string, ref array<ref RewardConfigEntry>>;
		PVPRewardConfigs = new map<string, ref array<ref RewardConfigEntry>>;
		SimpleCashRewards = new TrackingModSimpleCashReward();
		EnableServerwideMilestoneNotification = false;
		ServerwideMilestoneMessage = "%1 hat %2 %3 Kills erreicht!";
		EnableCrateSpawnOnMilestone = false;
		CrateSpawnRadius = 15.0;
		CrateContainerClassName = "WoodenCrate";
		EnableCrateProximityNotification = false;
		CrateProximityCheckInterval = 30.0;
		CrateProximityNotifyRadius = 50.0;
		CrateDespawnTime = 600.0;
	}
	
	void CreateDefaultMilestoneConfig()
	{
		array<int> wolfesMilestones;
		array<int> zombiesMilestones;

		if (!SimpleCashRewards)
			SimpleCashRewards = new TrackingModSimpleCashReward();
		array<int> playersMilestones;
		array<ref RewardConfigEntry> wolfesRewardConfigs;
		array<ref RewardConfigEntry> zombiesRewardConfigs;
		array<ref RewardConfigEntry> playersRewardConfigs;
		RewardConfigEntry entry;
		
		wolfesMilestones = new array<int>;
		wolfesMilestones.Insert(100);
		wolfesMilestones.Insert(200);
		wolfesMilestones.Insert(300);
		wolfesMilestones.Insert(500);
		wolfesMilestones.Insert(1000);
		PVEMilestones.Set("Wolfes", wolfesMilestones);
		
		zombiesMilestones = new array<int>;
		zombiesMilestones.Insert(50);
		zombiesMilestones.Insert(100);
		zombiesMilestones.Insert(200);
		zombiesMilestones.Insert(500);
		zombiesMilestones.Insert(1000);
		PVEMilestones.Set("Zombies", zombiesMilestones);
		
		playersMilestones = new array<int>;
		playersMilestones.Insert(10);
		playersMilestones.Insert(25);
		playersMilestones.Insert(50);
		playersMilestones.Insert(100);
		playersMilestones.Insert(200);
		PVPMilestones.Set("Players", playersMilestones);
		
		wolfesRewardConfigs = new array<ref RewardConfigEntry>;
		entry = new RewardConfigEntry();
		entry.Milestone = "100";
		entry.FileName = "Wolfes100.json";
		wolfesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "200";
		entry.FileName = "Wolfes200.json";
		wolfesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "300";
		entry.FileName = "Wolfes300.json";
		wolfesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "500";
		entry.FileName = "Wolfes500.json";
		wolfesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "1000";
		entry.FileName = "Wolfes1000.json";
		wolfesRewardConfigs.Insert(entry);
		PVERewardConfigs.Set("Wolfes", wolfesRewardConfigs);
		
		zombiesRewardConfigs = new array<ref RewardConfigEntry>;
		entry = new RewardConfigEntry();
		entry.Milestone = "50";
		entry.FileName = "Zombies50.json";
		zombiesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "100";
		entry.FileName = "Zombies100.json";
		zombiesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "200";
		entry.FileName = "Zombies200.json";
		zombiesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "500";
		entry.FileName = "Zombies500.json";
		zombiesRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "1000";
		entry.FileName = "Zombies1000.json";
		zombiesRewardConfigs.Insert(entry);
		PVERewardConfigs.Set("Zombies", zombiesRewardConfigs);
		
		playersRewardConfigs = new array<ref RewardConfigEntry>;
		entry = new RewardConfigEntry();
		entry.Milestone = "10";
		entry.FileName = "Players10.json";
		playersRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "25";
		entry.FileName = "Players25.json";
		playersRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "50";
		entry.FileName = "Players50.json";
		playersRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "100";
		entry.FileName = "Players100.json";
		playersRewardConfigs.Insert(entry);
		entry = new RewardConfigEntry();
		entry.Milestone = "200";
		entry.FileName = "Players200.json";
		playersRewardConfigs.Insert(entry);
		PVPRewardConfigs.Set("Players", playersRewardConfigs);
		
		SortRewardConfigArrays();
	}
	
	void CreateExampleRewardConfigFiles()
	{
		string rewardSystemDir;
		
		if (!GetGame().IsDedicatedServer())
			return;
		
		rewardSystemDir = RewardConfigFolder;
		if (rewardSystemDir == "")
			rewardSystemDir = TRACKING_MOD_REWARD_SYSTEM_DIR;
		
		if (!FileExist(rewardSystemDir))
		{
			CF_Directory.CreateDirectory(rewardSystemDir);
			TrackingMod.LogInfo("Created RewardSystem directory: " + rewardSystemDir);
		}
		
		CreateWolfesRewardConfig(rewardSystemDir, "Wolfes100.json", 100, 1000);
		CreateWolfesRewardConfig(rewardSystemDir, "Wolfes200.json", 200, 2000);
		CreateWolfesRewardConfig(rewardSystemDir, "Wolfes300.json", 300, 3000);
		CreateWolfesRewardConfig(rewardSystemDir, "Wolfes500.json", 500, 5000);
		CreateWolfesRewardConfig(rewardSystemDir, "Wolfes1000.json", 1000, 10000);
		
		CreateZombiesRewardConfig(rewardSystemDir, "Zombies50.json", 50, 500);
		CreateZombiesRewardConfig(rewardSystemDir, "Zombies100.json", 100, 1000);
		CreateZombiesRewardConfig(rewardSystemDir, "Zombies200.json", 200, 2000);
		CreateZombiesRewardConfig(rewardSystemDir, "Zombies500.json", 500, 5000);
		CreateZombiesRewardConfig(rewardSystemDir, "Zombies1000.json", 1000, 10000);
	}
	
	void CreateWolfesRewardConfig(string rewardSystemDir, string fileName, int milestone, int currencyAmount)
	{
		string filePath;
		TrackingModMilestoneRewardConfig exampleConfig;
		TrackingModRewardSection exampleSection;
		TrackingModRewardItem exampleItem;
		TrackingModRewardAttachment magAttachment;
		TrackingModRewardAttachment opticAttachment;
		TrackingModRewardAttachment opticBatteryAttachment;
		TrackingModRewardAttachment lightAttachment;
		TrackingModRewardAttachment lightBatteryAttachment;
		TrackingModCurrencyReward exampleCurrency;
		string weaponClass;
		string magClass;
		string opticClass;
		string lightClass;
		int magAmmo;
		
		filePath = rewardSystemDir + fileName;
		if (FileExist(filePath))
			return;
		
		exampleConfig = new TrackingModMilestoneRewardConfig();
		exampleSection = new TrackingModRewardSection();
		exampleSection.Name = "Wolf Hunter";
		exampleSection.SpawnChance = 100.0;
		exampleSection.ItemsMin = 1;
		exampleSection.ItemsMax = 1;
		exampleSection.LootItems = new array<ref TrackingModRewardItem>;
		
		if (milestone <= 100)
		{
			weaponClass = "Glock19";
			magClass = "Mag_Glock_15Rnd";
			magAmmo = 15;
			opticClass = "PistolOptic";
			lightClass = "UniversalLight";
		}
		else if (milestone <= 200)
		{
			weaponClass = "AKM";
			magClass = "Mag_AKM_30Rnd";
			magAmmo = 30;
			opticClass = "KobraOptic";
			lightClass = "UniversalLight";
		}
		else if (milestone <= 300)
		{
			weaponClass = "M4A1";
			magClass = "Mag_STANAG_30Rnd";
			magAmmo = 30;
			opticClass = "ACOGOptic";
			lightClass = "UniversalLight";
		}
		else if (milestone <= 500)
		{
			weaponClass = "AK101";
			magClass = "Mag_AK101_30Rnd";
			magAmmo = 30;
			opticClass = "PSO1Optic";
			lightClass = "UniversalLight";
		}
		else
		{
			weaponClass = "SVD";
			magClass = "Mag_SVD_10Rnd";
			magAmmo = 10;
			opticClass = "PSO11Optic";
			lightClass = "UniversalLight";
		}
		
		exampleItem = new TrackingModRewardItem();
		exampleItem.ItemClassName = weaponClass;
		exampleItem.SpawnChance = 100.0;
		exampleItem.HealthMin = 0.7;
		exampleItem.HealthMax = 1.0;
		exampleItem.Attachments = new array<ref TrackingModRewardAttachment>;
		
		magAttachment = new TrackingModRewardAttachment();
		magAttachment.ItemClassName = magClass;
		magAttachment.SpawnChance = 100.0;
		magAttachment.Amount = magAmmo;
		magAttachment.HealthMin = 0.8;
		magAttachment.HealthMax = 1.0;
		magAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
		exampleItem.Attachments.Insert(magAttachment);
		
		opticAttachment = new TrackingModRewardAttachment();
		opticAttachment.ItemClassName = opticClass;
		opticAttachment.SpawnChance = 80.0;
		opticAttachment.HealthMin = 0.8;
		opticAttachment.HealthMax = 1.0;
		opticAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
		
		opticBatteryAttachment = new TrackingModRewardAttachment();
		opticBatteryAttachment.ItemClassName = "BatteryD";
		opticBatteryAttachment.SpawnChance = 90.0;
		opticBatteryAttachment.HealthMin = 0.9;
		opticBatteryAttachment.HealthMax = 1.0;
		opticBatteryAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
		opticAttachment.Attachments.Insert(opticBatteryAttachment);
		exampleItem.Attachments.Insert(opticAttachment);
		
		lightAttachment = new TrackingModRewardAttachment();
		lightAttachment.ItemClassName = lightClass;
		lightAttachment.SpawnChance = 70.0;
		lightAttachment.HealthMin = 0.8;
		lightAttachment.HealthMax = 1.0;
		lightAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
		
		lightBatteryAttachment = new TrackingModRewardAttachment();
		lightBatteryAttachment.ItemClassName = "BatteryD";
		lightBatteryAttachment.SpawnChance = 85.0;
		lightBatteryAttachment.HealthMin = 0.9;
		lightBatteryAttachment.HealthMax = 1.0;
		lightBatteryAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
		lightAttachment.Attachments.Insert(lightBatteryAttachment);
		exampleItem.Attachments.Insert(lightAttachment);
		
		exampleSection.LootItems.Insert(exampleItem);
		exampleConfig.RewardItems.Insert(exampleSection);
		
		exampleCurrency = new TrackingModCurrencyReward();
		exampleCurrency.ClassName = "Hryvnia";
		exampleCurrency.SpawnChance = 100.0;
		exampleCurrency.Amount = currencyAmount;
		exampleConfig.CurrencyRewards.Insert(exampleCurrency);
		
		JsonFileLoader<TrackingModMilestoneRewardConfig>.JsonSaveFile(filePath, exampleConfig);
		TrackingMod.LogInfo("Created example reward config: " + filePath);
	}
	
	void CreateZombiesRewardConfig(string rewardSystemDir, string fileName, int milestone, int currencyAmount)
	{
		string filePath;
		TrackingModMilestoneRewardConfig exampleConfig;
		TrackingModRewardSection exampleSection;
		TrackingModRewardItem exampleItem;
		TrackingModRewardItem exampleItem2;
		TrackingModRewardItem exampleItem3;
		TrackingModRewardAttachment exampleAttachment;
		TrackingModCurrencyReward exampleCurrency;
		
		filePath = rewardSystemDir + fileName;
		if (FileExist(filePath))
			return;
		
		exampleConfig = new TrackingModMilestoneRewardConfig();
		exampleSection = new TrackingModRewardSection();
		exampleSection.Name = "Zombie Slayer";
		exampleSection.SpawnChance = 100.0;
		if (milestone <= 100)
		{
			exampleSection.ItemsMin = 2;
			exampleSection.ItemsMax = 3;
		}
		else if (milestone <= 200)
		{
			exampleSection.ItemsMin = 3;
			exampleSection.ItemsMax = 4;
		}
		else
		{
			exampleSection.ItemsMin = 4;
			exampleSection.ItemsMax = 5;
		}
		exampleSection.LootItems = new array<ref TrackingModRewardItem>;
		
		exampleItem = new TrackingModRewardItem();
		exampleItem.ItemClassName = "BandageDressing";
		exampleItem.SpawnChance = 80.0;
		exampleItem.Amount = 3;
		exampleItem.HealthMin = 0.9;
		exampleItem.HealthMax = 1.0;
		exampleItem.Attachments = new array<ref TrackingModRewardAttachment>;
		exampleSection.LootItems.Insert(exampleItem);
		
		exampleItem2 = new TrackingModRewardItem();
		exampleItem2.ItemClassName = "Morphine";
		exampleItem2.SpawnChance = 60.0;
		exampleItem2.Amount = 1;
		exampleItem2.HealthMin = 0.9;
		exampleItem2.HealthMax = 1.0;
		exampleItem2.Attachments = new array<ref TrackingModRewardAttachment>;
		exampleSection.LootItems.Insert(exampleItem2);
		
		exampleItem3 = new TrackingModRewardItem();
		exampleItem3.ItemClassName = "Epinephrine";
		exampleItem3.SpawnChance = 50.0;
		exampleItem3.Amount = 1;
		exampleItem3.HealthMin = 0.9;
		exampleItem3.HealthMax = 1.0;
		exampleItem3.Attachments = new array<ref TrackingModRewardAttachment>;
		exampleSection.LootItems.Insert(exampleItem3);
		
		if (milestone >= 200)
		{
			exampleItem = new TrackingModRewardItem();
			exampleItem.ItemClassName = "PlateCarrierVest";
			exampleItem.SpawnChance = 40.0;
			exampleItem.HealthMin = 0.6;
			exampleItem.HealthMax = 1.0;
			exampleItem.Attachments = new array<ref TrackingModRewardAttachment>;
			
			exampleAttachment = new TrackingModRewardAttachment();
			exampleAttachment.ItemClassName = "PlateCarrierPouches";
			exampleAttachment.SpawnChance = 70.0;
			exampleAttachment.HealthMin = 0.8;
			exampleAttachment.HealthMax = 1.0;
			exampleAttachment.Attachments = new array<ref TrackingModRewardAttachment>;
			exampleItem.Attachments.Insert(exampleAttachment);
			exampleSection.LootItems.Insert(exampleItem);
		}
		
		exampleConfig.RewardItems.Insert(exampleSection);
		
		exampleCurrency = new TrackingModCurrencyReward();
		exampleCurrency.ClassName = "Hryvnia";
		exampleCurrency.SpawnChance = 100.0;
		exampleCurrency.Amount = currencyAmount;
		exampleConfig.CurrencyRewards.Insert(exampleCurrency);
		
		JsonFileLoader<TrackingModMilestoneRewardConfig>.JsonSaveFile(filePath, exampleConfig);
		TrackingMod.LogInfo("Created example reward config: " + filePath);
	}
	
	void SortRewardConfigArrays()
	{
		string categoryID;
		array<ref RewardConfigEntry> rewardConfigs;
		int i;
		int j;
		int k;
		int milestoneValue;
		int compareValue;
		RewardConfigEntry tempEntry;
		
		if (PVERewardConfigs)
		{
			for (i = 0; i < PVERewardConfigs.Count(); i++)
			{
				categoryID = PVERewardConfigs.GetKey(i);
				rewardConfigs = PVERewardConfigs.GetElement(i);
				if (rewardConfigs)
				{
					for (j = 0; j < rewardConfigs.Count() - 1; j++)
					{
						for (k = j + 1; k < rewardConfigs.Count(); k++)
						{
							milestoneValue = rewardConfigs.Get(j).Milestone.ToInt();
							compareValue = rewardConfigs.Get(k).Milestone.ToInt();
							if (milestoneValue > compareValue)
							{
								tempEntry = rewardConfigs.Get(j);
								rewardConfigs.Set(j, rewardConfigs.Get(k));
								rewardConfigs.Set(k, tempEntry);
							}
						}
					}
				}
			}
		}
		
		if (PVPRewardConfigs)
		{
			for (i = 0; i < PVPRewardConfigs.Count(); i++)
			{
				categoryID = PVPRewardConfigs.GetKey(i);
				rewardConfigs = PVPRewardConfigs.GetElement(i);
				if (rewardConfigs)
				{
					for (j = 0; j < rewardConfigs.Count() - 1; j++)
					{
						for (k = j + 1; k < rewardConfigs.Count(); k++)
						{
							milestoneValue = rewardConfigs.Get(j).Milestone.ToInt();
							compareValue = rewardConfigs.Get(k).Milestone.ToInt();
							if (milestoneValue > compareValue)
							{
								tempEntry = rewardConfigs.Get(j);
								rewardConfigs.Set(j, rewardConfigs.Get(k));
								rewardConfigs.Set(k, tempEntry);
							}
						}
					}
				}
			}
		}
	}
	
	string GetRewardConfigFileName(string categoryID, string milestoneStr, bool isPvE)
	{
		array<ref RewardConfigEntry> rewardConfigs;
		int i;
		RewardConfigEntry entry;
		
		if (isPvE && PVERewardConfigs && PVERewardConfigs.Contains(categoryID))
		{
			rewardConfigs = PVERewardConfigs.Get(categoryID);
			if (rewardConfigs)
			{
				for (i = 0; i < rewardConfigs.Count(); i++)
				{
					entry = rewardConfigs.Get(i);
					if (entry && entry.Milestone == milestoneStr)
					{
						return entry.FileName;
					}
				}
			}
		}
		else if (!isPvE && PVPRewardConfigs && PVPRewardConfigs.Contains(categoryID))
		{
			rewardConfigs = PVPRewardConfigs.Get(categoryID);
			if (rewardConfigs)
			{
				for (i = 0; i < rewardConfigs.Count(); i++)
				{
					entry = rewardConfigs.Get(i);
					if (entry && entry.Milestone == milestoneStr)
					{
						return entry.FileName;
					}
				}
			}
		}
		
		return "";
	}
	
	void SaveConfig(bool sortMaps = true)
	{
		string actualFilePath;
		
		if (!GetGame().IsDedicatedServer())
		{
			return;
		}
		TrackingModData.CheckDirectories();
		if (sortMaps)
		{
			SortRewardConfigArrays();
		}
		actualFilePath = TRACKING_MOD_DATA_DIR + "TrackingModRewardConfig.json";
		Print("[TrackingMod] Saving TrackingModRewardConfig to: " + actualFilePath);
		TrackingMod.LogInfo("Saving TrackingModRewardConfig to: " + actualFilePath);
		JsonFileLoader<TrackingModRewardConfig>.JsonSaveFile(actualFilePath, this);
	}
	
	static TrackingModRewardConfig LoadConfig()
	{
		string actualFilePath;
		
		if (!GetGame().IsDedicatedServer())
		{
			return null;
		}
		TrackingModData.CheckDirectories();
		if (!FileExist(TRACKING_MOD_REWARD_SYSTEM_DIR))
		{
			CF_Directory.CreateDirectory(TRACKING_MOD_REWARD_SYSTEM_DIR);
			TrackingMod.LogInfo("Created RewardSystem directory: " + TRACKING_MOD_REWARD_SYSTEM_DIR);
		}
		TrackingModRewardConfig config;
		actualFilePath = TRACKING_MOD_DATA_DIR + "TrackingModRewardConfig.json";
		if (FileExist(actualFilePath))
		{
			config = new TrackingModRewardConfig();
			JsonFileLoader<TrackingModRewardConfig>.JsonLoadFile(actualFilePath, config);
			Print("[TrackingMod] TrackingModRewardConfig loaded from: " + actualFilePath);
			TrackingMod.LogInfo("TrackingModRewardConfig loaded from file: " + actualFilePath);
			config.SortRewardConfigArrays();
			config.SaveConfig(false);
		}
		else
		{
			config = new TrackingModRewardConfig();
			Print("[TrackingMod] TrackingModRewardConfig.json not found. Creating default config.");
			TrackingMod.LogInfo("TrackingModRewardConfig.json not found. Creating default config.");
			config.CreateDefaultMilestoneConfig();
			config.SaveConfig(true);
		}
		config.CreateExampleRewardConfigFiles();
		actualFilePath = TRACKING_MOD_DATA_DIR + "TrackingModRewardConfig.json";
		if (FileExist(actualFilePath))
		{
			Print("[TrackingMod] TrackingModRewardConfig.json saved/created successfully at: " + actualFilePath);
			TrackingMod.LogInfo("TrackingModRewardConfig.json saved/created successfully at: " + actualFilePath);
		}
		else
		{
			Print("[TrackingMod] ERROR: TrackingModRewardConfig.json was NOT created! Expected path: " + actualFilePath);
			TrackingMod.LogCritical("ERROR: TrackingModRewardConfig.json was NOT created! Expected path: " + actualFilePath);
		}
		return config;
	}
}

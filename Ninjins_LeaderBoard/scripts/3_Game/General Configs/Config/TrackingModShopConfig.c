const string TRACKING_MOD_SHOP_CONFIG_FILE = TRACKING_MOD_DATA_DIR + "ShopConfig.json";
ref TrackingModShopConfig g_TrackingModShopConfig;

class TrackingModShopItem
{
	string ItemClassName;
	string DisplayName;
	int PricePvE;
	int PricePvP;
	int MaxPerPurchase;
	ref array<ref TrackingModRewardAttachment> Attachments;

	void TrackingModShopItem()
	{
		ItemClassName = "";
		DisplayName = "";
		PricePvE = 0;
		PricePvP = 0;
		MaxPerPurchase = 1;
		Attachments = new array<ref TrackingModRewardAttachment>();
	}
}

class TrackingModShopCrate
{
	string CrateName;
	string DisplayName;
	int PricePvE;
	int PricePvP;
	ref array<ref TrackingModRewardItem> PossibleItems;
	int ItemsMin;
	int ItemsMax;

	void TrackingModShopCrate()
	{
		CrateName = "";
		DisplayName = "";
		PricePvE = 0;
		PricePvP = 0;
		PossibleItems = new array<ref TrackingModRewardItem>();
		ItemsMin = 1;
		ItemsMax = 3;
	}
}

class TrackingModShopConfig
{
	bool EnableShop;
	string PointType;
	ref array<ref TrackingModShopItem> ShopItems;
	ref array<ref TrackingModShopCrate> ShopCrates;

	void TrackingModShopConfig()
	{
		EnableShop = false;
		PointType = "Both";
		ShopItems = new array<ref TrackingModShopItem>();
		ShopCrates = new array<ref TrackingModShopCrate>();
	}

	void SaveConfig()
	{
		if (!g_Game || !g_Game.IsDedicatedServer())
			return;
		TrackingModData.CheckDirectories();
		JsonFileLoader<TrackingModShopConfig>.JsonSaveFile(TRACKING_MOD_SHOP_CONFIG_FILE, this);
	}

	static TrackingModShopConfig LoadConfig()
	{
		TrackingModShopConfig config;

		if (!g_Game || !g_Game.IsDedicatedServer())
			return null;

		TrackingModData.CheckDirectories();
		if (FileExist(TRACKING_MOD_SHOP_CONFIG_FILE))
		{
			config = new TrackingModShopConfig();
			JsonFileLoader<TrackingModShopConfig>.JsonLoadFile(TRACKING_MOD_SHOP_CONFIG_FILE, config);
			Print("[TrackingMod] ShopConfig loaded from: " + TRACKING_MOD_SHOP_CONFIG_FILE);
		}
		else
		{
			config = new TrackingModShopConfig();
			config.CreateDefaultShopConfig();
			config.SaveConfig();
			Print("[TrackingMod] ShopConfig.json not found. Created default.");
		}
		return config;
	}

	void CreateDefaultShopConfig()
	{
		TrackingModShopItem exampleItem;

		exampleItem = new TrackingModShopItem();
		exampleItem.ItemClassName = "BandageDressing";
		exampleItem.DisplayName = "Bandage";
		exampleItem.PricePvE = 50;
		exampleItem.PricePvP = 50;
		exampleItem.MaxPerPurchase = 5;
		ShopItems.Insert(exampleItem);

		exampleItem = new TrackingModShopItem();
		exampleItem.ItemClassName = "Morphine";
		exampleItem.DisplayName = "Morphine";
		exampleItem.PricePvE = 200;
		exampleItem.PricePvP = 200;
		exampleItem.MaxPerPurchase = 3;
		ShopItems.Insert(exampleItem);
	}
}

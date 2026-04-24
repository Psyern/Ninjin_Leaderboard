class PVPCategory
{
	string CategoryID;
	string DisplayName;
	ref array<string> ClassNames;

	void PVPCategory()
	{
		CategoryID = "";
		DisplayName = "";
		ClassNames = new array<string>();
	}

	string ExtractConfiguredClassName(string configuredClassName)
	{
		int firstColonIndex;
		
		firstColonIndex = configuredClassName.IndexOf(":");
		if (firstColonIndex == -1)
			return configuredClassName;
		
		return configuredClassName.Substring(0, firstColonIndex);
	}

	bool MatchesClass(string className, Object entity = null)
	{
		string configuredClassName;
		string categoryClassName;
		string prefix;
		string nextChar;
		int i;
		
		if (!ClassNames || ClassNames.Count() == 0)
			return false;
		for (i = 0; i < ClassNames.Count(); i++)
		{
			configuredClassName = ClassNames[i];
			categoryClassName = ExtractConfiguredClassName(configuredClassName);
			if (categoryClassName == className)
				return true;
			if (className.Length() > categoryClassName.Length())
			{
				prefix = className.Substring(0, categoryClassName.Length());
				if (prefix == categoryClassName)
				{
					nextChar = className.Substring(categoryClassName.Length(), 1);
					if (nextChar == "_")
						return true;
				}
			}
			if (entity && entity.IsKindOf(categoryClassName))
				return true;
			if (MatchesBaseClassByConfig(className, categoryClassName))
				return true;
		}
		return false;
	}
	bool MatchesBaseClassByConfig(string className, string baseClassName)
	{
		if (className == baseClassName)
			return true;
		if (!GetGame().ConfigIsExisting("CfgVehicles " + className))
			return false;
		if (!GetGame().ConfigIsExisting("CfgVehicles " + baseClassName))
			return false;
		string configPath = "CfgVehicles " + className;
		string parentPath = configPath + " parent";
		if (GetGame().ConfigIsExisting(parentPath))
		{
			string parentClass;
			GetGame().ConfigGetText(parentPath, parentClass);
			if (parentClass == baseClassName)
				return true;
			if (MatchesBaseClassByConfig(parentClass, baseClassName))
				return true;
		}
		int childCount = GetGame().ConfigGetChildrenCount(configPath);
		for (int i = 0; i < childCount; i++)
		{
			string childName;
			GetGame().ConfigGetChildName(configPath, i, childName);
			if (childName == "parent")
			{
				string parentClassFromChild;
				GetGame().ConfigGetText(configPath + " " + childName, parentClassFromChild);
				if (parentClassFromChild == baseClassName)
					return true;
				if (MatchesBaseClassByConfig(parentClassFromChild, baseClassName))
					return true;
			}
		}
		return false;
	}
}
class PVPCategoryConfigJson
{
	autoptr array<ref PVPCategory> Categories;
	int PVPDeathPenaltyPoints;
	void PVPCategoryConfigJson()
	{
		Categories = new array<ref PVPCategory>();
		PVPDeathPenaltyPoints = 1;
	}
}
class PVPCategoryConfig
{
	protected static ref PVPCategoryConfig Instance;
	protected ref array<ref PVPCategory> m_Categories;
	protected ref map<string, ref PVPCategory> m_CategoryMap;
	protected int m_PVPDeathPenaltyPoints;
	const string PVP_CATEGORIES_FILE = TRACKING_MOD_DATA_DIR + "PVP_Categories.json";
	void PVPCategoryConfig()
	{
		m_Categories = new array<ref PVPCategory>();
		m_CategoryMap = new map<string, ref PVPCategory>();
		LoadCategories();
	}
	static PVPCategoryConfig GetInstance(bool reload = false)
	{
		if (!Instance)
		{
			Instance = new PVPCategoryConfig();
		}
		if (reload)
		{
			Instance.LoadCategories();
		}
		return Instance;
	}
	void LoadCategories()
	{
		m_Categories.Clear();
		m_CategoryMap.Clear();
		if (!FileExist(PVP_CATEGORIES_FILE))
		{
			CreateDefaultCategories();
			return;
		}
		PVPCategoryConfigJson jsonData = new PVPCategoryConfigJson();
		if (FileExist(PVP_CATEGORIES_FILE))
		{
			JsonFileLoader<PVPCategoryConfigJson>.JsonLoadFile(PVP_CATEGORIES_FILE, jsonData);
		}
		if (jsonData)
		{
			if (jsonData.Categories)
			{
				for (int i = 0; i < jsonData.Categories.Count(); i++)
				{
					PVPCategory category = jsonData.Categories[i];
					if (category && category.CategoryID != "")
					{
						m_Categories.Insert(category);
						m_CategoryMap.Set(category.CategoryID, category);
					}
				}
			}
			m_PVPDeathPenaltyPoints = jsonData.PVPDeathPenaltyPoints;
		}
		Print("[TrackingMod] Loaded " + m_Categories.Count().ToString() + " PVP categories, PVPDeathPenaltyPoints: " + m_PVPDeathPenaltyPoints.ToString());
	}
	void CreateDefaultCategories()
	{
		PVPCategoryConfigJson defaultData = new PVPCategoryConfigJson();
		PVPCategory players = new PVPCategory();
		players.CategoryID = "Players";
		players.ClassNames.Insert("PlayerBase:100:1");
		defaultData.Categories.Insert(players);
		defaultData.PVPDeathPenaltyPoints = 10;
		JsonFileLoader<PVPCategoryConfigJson>.JsonSaveFile(PVP_CATEGORIES_FILE, defaultData);

		// Populate in-memory state directly from defaults instead of recursing
		// into LoadCategories(). On clients without write access to $profile,
		// JsonSaveFile fails silently, so a re-Load would recurse infinitely.
		int i;
		PVPCategory category;
		for (i = 0; i < defaultData.Categories.Count(); i++)
		{
			category = defaultData.Categories[i];
			if (category && category.CategoryID != "")
			{
				m_Categories.Insert(category);
				m_CategoryMap.Set(category.CategoryID, category);
			}
		}
		m_PVPDeathPenaltyPoints = defaultData.PVPDeathPenaltyPoints;
	}
	array<ref PVPCategory> GetCategories()
	{
		return m_Categories;
	}
	PVPCategory GetCategory(string categoryID)
	{
		if (m_CategoryMap.Contains(categoryID))
			return m_CategoryMap.Get(categoryID);
		return null;
	}
	array<string> GetCategoryIDsForClass(string className, Object entity = null)
	{
		array<string> matchingCategories = new array<string>();
		for (int i = 0; i < m_Categories.Count(); i++)
		{
			PVPCategory category = m_Categories[i];
			if (category && category.MatchesClass(className, entity))
			{
				matchingCategories.Insert(category.CategoryID);
			}
		}
		return matchingCategories;
	}

	string GetPrimaryPlayerCategoryID()
	{
		int i;
		PVPCategory category;
		string classNameEntry;
		string configuredClassName;
		
		if (m_Categories)
		{
			for (i = 0; i < m_Categories.Count(); i++)
			{
				category = m_Categories.Get(i);
				if (!category || !category.ClassNames)
					continue;
				
				for (int j = 0; j < category.ClassNames.Count(); j++)
				{
					classNameEntry = category.ClassNames.Get(j);
					configuredClassName = category.ExtractConfiguredClassName(classNameEntry);
					if (configuredClassName == "PlayerBase")
						return category.CategoryID;
				}
			}
			
			if (m_Categories.Count() > 0 && m_Categories.Get(0))
				return m_Categories.Get(0).CategoryID;
		}
		
		return "Players";
	}

	bool HasCategory(string categoryID)
	{
		return m_CategoryMap.Contains(categoryID);
	}
	int GetPVPDeathPenaltyPoints()
	{
		return m_PVPDeathPenaltyPoints;
	}

	void ApplyAdminData(TrackingModPVPAdminData data)
	{
		PVPCategory sourceCategory;
		PVPCategory clonedCategory;
		int i;
		int j;

		m_Categories.Clear();
		m_CategoryMap.Clear();
		m_PVPDeathPenaltyPoints = 0;

		if (data)
		{
			m_PVPDeathPenaltyPoints = Math.Max(0, data.PVPDeathPenaltyPoints);
			if (data.Categories)
			{
				for (i = 0; i < data.Categories.Count(); i++)
				{
					sourceCategory = data.Categories.Get(i);
					if (!sourceCategory || sourceCategory.CategoryID == "")
						continue;

					clonedCategory = new PVPCategory();
					clonedCategory.CategoryID = sourceCategory.CategoryID;
					clonedCategory.DisplayName = sourceCategory.DisplayName;
					for (j = 0; j < sourceCategory.ClassNames.Count(); j++)
						clonedCategory.ClassNames.Insert(sourceCategory.ClassNames.Get(j));

					m_Categories.Insert(clonedCategory);
					m_CategoryMap.Set(clonedCategory.CategoryID, clonedCategory);
				}
			}
		}

		SaveCategories();
	}

	void SaveCategories()
	{
		PVPCategoryConfigJson jsonData;
		PVPCategory sourceCategory;
		PVPCategory clonedCategory;
		int i;
		int j;

		jsonData = new PVPCategoryConfigJson();
		jsonData.PVPDeathPenaltyPoints = m_PVPDeathPenaltyPoints;
		for (i = 0; i < m_Categories.Count(); i++)
		{
			sourceCategory = m_Categories.Get(i);
			if (!sourceCategory || sourceCategory.CategoryID == "")
				continue;

			clonedCategory = new PVPCategory();
			clonedCategory.CategoryID = sourceCategory.CategoryID;
			clonedCategory.DisplayName = sourceCategory.DisplayName;
			for (j = 0; j < sourceCategory.ClassNames.Count(); j++)
				clonedCategory.ClassNames.Insert(sourceCategory.ClassNames.Get(j));

			jsonData.Categories.Insert(clonedCategory);
		}

		JsonFileLoader<PVPCategoryConfigJson>.JsonSaveFile(PVP_CATEGORIES_FILE, jsonData);
	}
}
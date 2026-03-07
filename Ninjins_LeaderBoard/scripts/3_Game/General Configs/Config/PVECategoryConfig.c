class CategoryMatch
{
	string CategoryID;
	int Multiplier;
}
class PVECategory
{
	string CategoryID;
	string ClassNamePreview;
	ref array<string> ClassNames;
	void PVECategory()
	{
		CategoryID = "";
		ClassNamePreview = "";
		ClassNames = new array<string>();
	}
	bool MatchesClass(string className, Object entity = null, out int basePoints = 0, out int multiplier = 0)
	{
		basePoints = 0;
		multiplier = 0;
		if (!ClassNames || ClassNames.Count() == 0)
			return false;
		for (int i = 0; i < ClassNames.Count(); i++)
		{
			string categoryClassNameWithSettings = ClassNames[i];
			string categoryClassName = categoryClassNameWithSettings;
			int classBasePoints = 0;
			int classMultiplier = 1;
			int firstColonIndex = categoryClassNameWithSettings.IndexOf(":");
			if (firstColonIndex != -1)
			{
				categoryClassName = categoryClassNameWithSettings.Substring(0, firstColonIndex);
				string remainingStr = categoryClassNameWithSettings.Substring(firstColonIndex + 1, categoryClassNameWithSettings.Length() - (firstColonIndex + 1));
				int secondColonIndex = remainingStr.IndexOf(":");
				if (secondColonIndex == -1)
				{
					classBasePoints = remainingStr.ToInt();
					classMultiplier = 1;
				}
				else
				{
					string basePointsStr = remainingStr.Substring(0, secondColonIndex);
					string multiplierStr = remainingStr.Substring(secondColonIndex + 1, remainingStr.Length() - (secondColonIndex + 1));
					classBasePoints = basePointsStr.ToInt();
					classMultiplier = multiplierStr.ToInt();
				}
			}
			if (categoryClassName == className)
			{
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
			if (categoryClassName == "eAIBase" && className.Length() >= 4)
			{
				string eAIPrefix = className.Substring(0, 4);
				if (eAIPrefix == "eAI_")
				{
					basePoints = classBasePoints;
					multiplier = classMultiplier;
					return true;
				}
			}
			if (className.Length() > categoryClassName.Length())
			{
				string prefix = className.Substring(0, categoryClassName.Length());
				if (prefix == categoryClassName)
				{
					string nextChar = className.Substring(categoryClassName.Length(), 1);
					if (nextChar == "_")
					{
						basePoints = classBasePoints;
						multiplier = classMultiplier;
						return true;
					}
				}
			}
			if (entity && MatchesBaseClassByEntity(entity, categoryClassName))
			{
				Print("[TrackingMod] Matched " + className + " to " + categoryClassName + " via entity check (IsKindOf)");
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
			if (MatchesBaseClassByConfig(className, categoryClassName))
			{
				Print("[TrackingMod] Matched " + className + " to " + categoryClassName + " via config check");
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
		}
		return false;
	}
	bool MatchesClassExact(string className, Object entity = null, out int basePoints = 0, out int multiplier = 0)
	{
		basePoints = 0;
		multiplier = 0;
		if (!ClassNames || ClassNames.Count() == 0)
			return false;
		for (int i = 0; i < ClassNames.Count(); i++)
		{
			string categoryClassNameWithSettings = ClassNames[i];
			string categoryClassName = categoryClassNameWithSettings;
			int classBasePoints = 0;
			int classMultiplier = 1;
			int firstColonIndex = categoryClassNameWithSettings.IndexOf(":");
			if (firstColonIndex != -1)
			{
				categoryClassName = categoryClassNameWithSettings.Substring(0, firstColonIndex);
				string remainingStr = categoryClassNameWithSettings.Substring(firstColonIndex + 1, categoryClassNameWithSettings.Length() - (firstColonIndex + 1));
				int secondColonIndex = remainingStr.IndexOf(":");
				if (secondColonIndex == -1)
				{
					classBasePoints = remainingStr.ToInt();
					classMultiplier = 1;
				}
				else
				{
					string basePointsStr = remainingStr.Substring(0, secondColonIndex);
					string multiplierStr = remainingStr.Substring(secondColonIndex + 1, remainingStr.Length() - (secondColonIndex + 1));
					classBasePoints = basePointsStr.ToInt();
					classMultiplier = multiplierStr.ToInt();
				}
			}
			if (categoryClassName == className)
			{
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
		}
		return false;
	}
	
	bool MatchesClassBase(string className, Object entity = null, out int basePoints = 0, out int multiplier = 0)
	{
		basePoints = 0;
		multiplier = 0;
		if (!ClassNames || ClassNames.Count() == 0)
			return false;
		for (int i = 0; i < ClassNames.Count(); i++)
		{
			string categoryClassNameWithSettings = ClassNames[i];
			string categoryClassName = categoryClassNameWithSettings;
			int classBasePoints = 0;
			int classMultiplier = 1;
			int firstColonIndex = categoryClassNameWithSettings.IndexOf(":");
			if (firstColonIndex != -1)
			{
				categoryClassName = categoryClassNameWithSettings.Substring(0, firstColonIndex);
				string remainingStr = categoryClassNameWithSettings.Substring(firstColonIndex + 1, categoryClassNameWithSettings.Length() - (firstColonIndex + 1));
				int secondColonIndex = remainingStr.IndexOf(":");
				if (secondColonIndex == -1)
				{
					classBasePoints = remainingStr.ToInt();
					classMultiplier = 1;
				}
				else
				{
					string basePointsStr = remainingStr.Substring(0, secondColonIndex);
					string multiplierStr = remainingStr.Substring(secondColonIndex + 1, remainingStr.Length() - (secondColonIndex + 1));
					classBasePoints = basePointsStr.ToInt();
					classMultiplier = multiplierStr.ToInt();
				}
			}
			if (categoryClassName == className)
				continue;
			if (className.Length() > categoryClassName.Length())
			{
				string prefix = className.Substring(0, categoryClassName.Length());
				if (prefix == categoryClassName)
				{
					string nextChar = className.Substring(categoryClassName.Length(), 1);
					if (nextChar == "_")
					{
						basePoints = classBasePoints;
						multiplier = classMultiplier;
						return true;
					}
				}
			}
			if (entity && MatchesBaseClassByEntity(entity, categoryClassName))
			{
				Print("[TrackingMod] Matched " + className + " to " + categoryClassName + " via entity check (IsKindOf)");
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
			if (MatchesBaseClassByConfig(className, categoryClassName))
			{
				Print("[TrackingMod] Matched " + className + " to " + categoryClassName + " via config check");
				basePoints = classBasePoints;
				multiplier = classMultiplier;
				return true;
			}
		}
		return false;
	}
	
	bool MatchesBaseClassByEntity(Object entity, string baseClassName)
	{
		if (!entity)
			return false;
		if (entity.IsKindOf(baseClassName))
			return true;
		if (baseClassName == "eAIBase")
		{
			string entityType = entity.GetType();
			if (entityType.Length() >= 4)
			{
				string prefix = entityType.Substring(0, 4);
				if (prefix == "eAI_")
					return true;
			}
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
class PVECategoryConfigJson
{
	autoptr array<ref PVECategory> Categories;
	int PVEDeathPenaltyPoints;
	void PVECategoryConfigJson()
	{
		Categories = new array<ref PVECategory>();
		PVEDeathPenaltyPoints = 1;
	}
}
class PVECategoryConfig
{
	protected static ref PVECategoryConfig Instance;
	protected ref array<ref PVECategory> m_Categories;
	protected ref map<string, ref PVECategory> m_CategoryMap;
	protected int m_PVEDeathPenaltyPoints;
	const string PVE_CATEGORIES_FILE = TRACKING_MOD_DATA_DIR + "PVE_Categories.json";
	void PVECategoryConfig()
	{
		m_Categories = new array<ref PVECategory>();
		m_CategoryMap = new map<string, ref PVECategory>();
		LoadCategories();
	}
	static PVECategoryConfig GetInstance(bool reload = false)
	{
		if (!Instance)
		{
			Instance = new PVECategoryConfig();
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
		if (!FileExist(PVE_CATEGORIES_FILE))
		{
			CreateDefaultCategories();
			return;
		}
		PVECategoryConfigJson jsonData = new PVECategoryConfigJson();
		if (FileExist(PVE_CATEGORIES_FILE))
		{
			JsonFileLoader<PVECategoryConfigJson>.JsonLoadFile(PVE_CATEGORIES_FILE, jsonData);
		}
		if (jsonData)
		{
			if (jsonData.Categories)
			{
				for (int i = 0; i < jsonData.Categories.Count(); i++)
				{
					PVECategory category = jsonData.Categories[i];
					if (category && category.CategoryID != "")
					{
						m_Categories.Insert(category);
						m_CategoryMap.Set(category.CategoryID, category);
					}
				}
			}
			m_PVEDeathPenaltyPoints = jsonData.PVEDeathPenaltyPoints;
		}
		Print("[TrackingMod] Loaded " + m_Categories.Count().ToString() + " PVE categories");
	}
	void CreateDefaultCategories()
	{
		PVECategoryConfigJson defaultData = new PVECategoryConfigJson();
		defaultData.PVEDeathPenaltyPoints = 5;
		PVECategory wolves = new PVECategory();
		wolves.CategoryID = "Wolves";
		wolves.ClassNamePreview = "Animal_CanisLupus_Grey";
		wolves.ClassNames.Insert("Animal_CanisLupus_Grey:10:5");
		wolves.ClassNames.Insert("Animal_CanisLupus_White:10:5");
		defaultData.Categories.Insert(wolves);
		PVECategory ai = new PVECategory();
		ai.CategoryID = "AI";
		ai.ClassNamePreview = "eAI_SurvivorM_Oliver";
		ai.ClassNames.Insert("eAI_SurvivorM_Oliver:10:5");
		defaultData.Categories.Insert(ai);
		PVECategory zombies1 = new PVECategory();
		zombies1.CategoryID = "Zombies";
		zombies1.ClassNamePreview = "ZmbM_HermitSkinny_Red";
		zombies1.ClassNames.Insert("ZmbM_HermitSkinny_Red:10:2");
		defaultData.Categories.Insert(zombies1);
		PVECategory animals = new PVECategory();
		animals.CategoryID = "Animals";
		animals.ClassNamePreview = "Animal_CapraHircus_Spooky";
		animals.ClassNames.Insert("AnimalBase:10:2");
		defaultData.Categories.Insert(animals);
		PVECategory players = new PVECategory();
		players.CategoryID = "Players";
		players.ClassNamePreview = "ZmbF_SurvivorNormal_Blue";
		players.ClassNames.Insert("PlayerBase:100:2");
		defaultData.Categories.Insert(players);
		PVECategory zombies2 = new PVECategory();
		zombies2.CategoryID = "ZombiesBased";
		zombies2.ClassNamePreview = "ZmbM_HermitSkinny_Red";
		zombies2.ClassNames.Insert("ZombieBase:10:2");
		defaultData.Categories.Insert(zombies2);
		JsonFileLoader<PVECategoryConfigJson>.JsonSaveFile(PVE_CATEGORIES_FILE, defaultData);
		LoadCategories();
	}
	array<ref PVECategory> GetCategories()
	{
		return m_Categories;
	}
	
	bool HasCategory(string categoryID)
	{
		if (!m_CategoryMap)
			return false;
		return m_CategoryMap.Contains(categoryID);
	}
	
	int GetPVEDeathPenaltyPoints()
	{
		return m_PVEDeathPenaltyPoints;
	}
	array<ref CategoryMatch> GetCategoryMatchesForClass(string className, Object entity = null)
	{
		array<ref CategoryMatch> exactMatches = new array<ref CategoryMatch>();
		array<ref CategoryMatch> baseClassMatches = new array<ref CategoryMatch>();
		
		for (int i = 0; i < m_Categories.Count(); i++)
		{
			PVECategory category = m_Categories[i];
			if (category)
			{
				int basePoints;
				int multiplier;
				bool isExactMatch = category.MatchesClassExact(className, entity, basePoints, multiplier);
				if (isExactMatch)
				{
					CategoryMatch match = new CategoryMatch();
					match.CategoryID = category.CategoryID;
					match.Multiplier = multiplier;
					exactMatches.Insert(match);
				}
				else
				{
					bool isBaseMatch = category.MatchesClassBase(className, entity, basePoints, multiplier);
					if (isBaseMatch)
					{
						CategoryMatch baseMatch = new CategoryMatch();
						baseMatch.CategoryID = category.CategoryID;
						baseMatch.Multiplier = multiplier;
						baseClassMatches.Insert(baseMatch);
					}
				}
			}
		}
		
		if (exactMatches.Count() > 0)
			return exactMatches;
		
		return baseClassMatches;
	}
}
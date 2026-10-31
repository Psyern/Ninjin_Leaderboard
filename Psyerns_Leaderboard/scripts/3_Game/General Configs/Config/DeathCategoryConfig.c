class DeathCategory
{
	string CategoryID;
	string DisplayName;
	void DeathCategory()
	{
		CategoryID = "";
		DisplayName = "";
	}
}

class DeathCategoryConfig
{
	protected static ref DeathCategoryConfig Instance;
	protected ref map<string, string> m_DisplayNames;
	
	void DeathCategoryConfig()
	{
		m_DisplayNames = new map<string, string>();
		InitializeDisplayNames();
	}
	
	static DeathCategoryConfig GetInstance(bool reload = false)
	{
		if (!Instance)
		{
			Instance = new DeathCategoryConfig();
		}
		return Instance;
	}
	
	void InitializeDisplayNames()
	{
		m_DisplayNames.Set("SelfInflicted", "Self Inflicted");
		m_DisplayNames.Set("KilledByPlayers", "Killed by Players");
		m_DisplayNames.Set("HeatDamage", "Heat Damage");
		m_DisplayNames.Set("Explosion", "Explosion");
		m_DisplayNames.Set("OtherEnvironment", "Other Environment");
	}
	
	DeathCategory GetCategory(string categoryID)
	{
		DeathCategory category;
		string displayName;
		
		category = new DeathCategory();
		category.CategoryID = categoryID;
		
		if (m_DisplayNames.Contains(categoryID))
		{
			displayName = m_DisplayNames.Get(categoryID);
			category.DisplayName = displayName;
		}
		else
		{
			category.DisplayName = categoryID;
		}
		
		return category;
	}
	
	bool HasCategory(string categoryID)
	{
		return m_DisplayNames.Contains(categoryID);
	}
}
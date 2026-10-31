static bool TrackingMod_IsBaseClass(Object entity, string baseClassName)
{
	if (!entity)
		return false;
	if (baseClassName == "ZombieBase")
	{
		if (entity.IsInherited(ZombieBase))
			return true;
	}
	if (baseClassName == "AnimalBase")
	{
		if (entity.IsInherited(AnimalBase))
			return true;
	}
	#ifdef EXPANSIONMODAI
	if (baseClassName == "eAIBase")
	{
		if (entity.IsInherited(eAIBase))
			return true;
	}
	#endif
	return false;
}
static bool TrackingMod_MatchesBaseClassByEntity(Object entity, string baseClassName)
{
	if (!entity)
		return false;
	return TrackingMod_IsBaseClass(entity, baseClassName);
}
static bool TrackingMod_MatchesClassForCategory(string className, Object entity, array<string> categoryClassNames)
{
	if (!categoryClassNames || categoryClassNames.Count() == 0)
		return false;
	for (int i = 0; i < categoryClassNames.Count(); i++)
	{
		string categoryClassName = categoryClassNames[i];
		if (categoryClassName == className)
			return true;
		if (className.Length() > categoryClassName.Length())
		{
			string prefix = className.Substring(0, categoryClassName.Length());
			if (prefix == categoryClassName)
			{
				string nextChar = className.Substring(categoryClassName.Length(), 1);
				if (nextChar == "_")
				{
					return true;
				}
			}
		}
		if (entity && TrackingMod_MatchesBaseClassByEntity(entity, categoryClassName))
			return true;
	}
	return false;
}
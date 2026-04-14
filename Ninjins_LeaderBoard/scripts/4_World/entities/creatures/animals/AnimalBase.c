modded class AnimalBase
{
	EntityAI m_TrackingMod_LastDamageSource;
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		bool applyDamage = super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (applyDamage)
			m_TrackingMod_LastDamageSource = source;
		return applyDamage;
	}
	override void EEKilled(Object killer)
	{
		if (g_Game.IsServer())
		{
			TrackAnimalKill(killer);
		}
		super.EEKilled(killer);
	}
	void TrackAnimalKill(Object killer)
	{
		PlayerBase killerPlayer;
		Man killerMan;
		PlayerIdentity killerIdentity;
		string killerID;
		string killerName;
		vector animalPosition;
		vector killerPosition;
		float killRange;
		string animalClassName;
		TrackingModData data;
		int damageType;
		string ammo;
		int killCause;
		
		if (killer == this)
			killer = m_TrackingMod_LastDamageSource;
		killerPlayer = PlayerBase.TrackingMod_ResolveExplosiveOwner(killer);
		if (!killerPlayer && killer && killer.IsInherited(EntityAI))
		{
			killerMan = EntityAI.Cast(killer).GetHierarchyRootPlayer();
			if (killerMan)
				killerPlayer = PlayerBase.Cast(killerMan);
		}
		if (!killerPlayer)
			return;
		killerIdentity = killerPlayer.GetIdentity();
		if (!killerIdentity)
			return;
		killerID = killerIdentity.GetPlainId();
		killerName = killerIdentity.GetName();
		animalPosition = GetPosition();
		killerPosition = killerPlayer.GetPosition();
		killRange = vector.Distance(animalPosition, killerPosition);
		animalClassName = GetType();
		data = TrackingModData.LoadData();
		damageType = -1;
		ammo = "";
		killCause = -1;
		if (killerPlayer)
		{
			damageType = killerPlayer.TrackingMod_GetLastDamageType();
			ammo = killerPlayer.TrackingMod_GetLastDamageAmmo();
			killCause = PlayerBase.GetKillCause(damageType, ammo, this);
		}
		data.AddAnimalKill(killerID, killerName, killRange, animalClassName, this, killCause);
		Print("[TrackingMod] Animal killed by player: " + killerName + " (ID: " + killerID + ") from " + killRange.ToString() + " meters (Class: " + animalClassName + ")");
		{
			PlayerDeathData cashCheckData;
			int totalAnimalKills;
			cashCheckData = data.GetPlayerData(killerID);
			if (cashCheckData && killerPlayer)
			{
				totalAnimalKills = cashCheckData.GetCategoryKills("Animals");
				TrackingModRewardHelper.CheckSimpleCashReward(killerPlayer, killerID, "Animals", totalAnimalKills);
			}
		}
	}
}
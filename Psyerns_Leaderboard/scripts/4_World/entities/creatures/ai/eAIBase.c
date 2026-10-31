#ifdef EXPANSIONMODAI
modded class eAIBase
{
	EntityAI m_TrackingMod_LastDamageSource;
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		bool applyDamage = super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (applyDamage && g_Game.IsServer())
			m_TrackingMod_LastDamageSource = source;
		return applyDamage;
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!g_Game.IsServer())
			return;
		TrackAIKill(killer);
	}
	void TrackAIKill(Object killer)
	{
		PlayerBase killerPlayer;
		Man killerMan;
		PlayerIdentity killerIdentity;
		string killerID;
		string killerName;
		vector aiPosition;
		vector killerPosition;
		float killRange;
		string aiClassName;
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
		aiPosition = GetPosition();
		killerPosition = killerPlayer.GetPosition();
		killRange = vector.Distance(aiPosition, killerPosition);
		aiClassName = GetType();
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
		data.AddAIKill(killerID, killerName, killRange, aiClassName, this, killCause);
		Print("[TrackingMod] AI killed by player: " + killerName + " (ID: " + killerID + ") from " + killRange.ToString() + " meters (Class: " + aiClassName + ")");
		{
			PlayerDeathData cashCheckData;
			int totalAIKills;
			cashCheckData = data.GetPlayerData(killerID);
			if (cashCheckData && killerPlayer)
			{
				totalAIKills = cashCheckData.GetCategoryKills("AI");
				TrackingModRewardHelper.CheckSimpleCashReward(killerPlayer, killerID, "AI", totalAIKills);
			}
		}

		#ifdef DME_War
		if (TrackingMod_IsDMEWBoss())
		{
			PlayerDeathData bossKillData;
			bossKillData = data.GetPlayerData(killerID);
			if (bossKillData)
			{
				bossKillData.WarBossKills = bossKillData.WarBossKills + 1;
				data.SavePlayerData(bossKillData, killerID);
				Print("[TrackingMod] DME-WAR Boss killed by " + killerName + " - WarBossKills=" + bossKillData.WarBossKills.ToString());
			}
		}
		#endif
	}

	#ifdef DME_War
	protected bool TrackingMod_IsDMEWBoss()
	{
		array<ref DMEBossSquad> squads;
		DMEBossSquad squad;
		int i;

		squads = DMEBossSpawner.s_Squads;
		if (!squads)
			return false;

		for (i = 0; i < squads.Count(); i++)
		{
			squad = squads[i];
			if (!squad)
				continue;
			if (squad.Boss == this)
				return true;
			if (squad.Wingman && squad.Wingman == this)
				return true;
		}

		return false;
	}
	#endif
}
#endif
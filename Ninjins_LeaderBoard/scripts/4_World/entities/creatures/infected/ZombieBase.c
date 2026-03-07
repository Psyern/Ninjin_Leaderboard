modded class ZombieBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!g_Game.IsServer())
			return;
		TrackZombieKill(killer);
	}
	void TrackZombieKill(Object killer)
	{
		PlayerBase killerPlayer;
		PlayerIdentity killerIdentity;
		string killerID;
		string killerName;
		vector zombiePosition;
		vector killerPosition;
		float killRange;
		string zombieClassName;
		TrackingModData data;
		int damageType;
		string ammo;
		int killCause;
		
		killerPlayer = PlayerBase.TrackingMod_ResolveExplosiveOwner(killer);
		if (!killerPlayer && killer && killer.IsInherited(EntityAI))
		{
			killerPlayer = PlayerBase.Cast(EntityAI.Cast(killer).GetHierarchyRootPlayer());
		}
		if (!killerPlayer)
			return;
		killerIdentity = killerPlayer.GetIdentity();
		if (!killerIdentity)
			return;
		killerID = killerIdentity.GetPlainId();
		killerName = killerIdentity.GetName();
		zombiePosition = GetPosition();
		killerPosition = killerPlayer.GetPosition();
		killRange = vector.Distance(zombiePosition, killerPosition);
		zombieClassName = GetType();
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
		data.AddZombieKill(killerID, killerName, killRange, zombieClassName, this, killCause);
		Print("[TrackingMod] Zombie killed by player: " + killerName + " (ID: " + killerID + ") from " + killRange.ToString() + " meters (Class: " + zombieClassName + ")");
	}
}
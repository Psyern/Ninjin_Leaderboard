modded class Grenade_Base
{
	private PlayerBase m_TrackingMod_Thrower;
	void TrackingMod_SetThrower(PlayerBase thrower)
	{
		m_TrackingMod_Thrower = thrower;
	}
	PlayerBase TrackingMod_GetThrower()
	{
		return m_TrackingMod_Thrower;
	}
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (!m_TrackingMod_Thrower && source)
		{
			PlayerBase attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (attacker)
			{
				TrackingMod_SetThrower(attacker);
			}
		}
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}
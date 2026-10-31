modded class ExplosivesBase
{
	protected PlayerBase m_TrackingMod_Placer;
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		PlayerBase placer = PlayerBase.Cast(player);
		if (placer)
		{
			TrackingMod_SetPlacer(placer);
		}
	}
	void TrackingMod_SetPlacer(PlayerBase placer)
	{
		m_TrackingMod_Placer = placer;
	}
	PlayerBase TrackingMod_GetPlacer()
	{
		return m_TrackingMod_Placer;
	}
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (!m_TrackingMod_Placer && source)
		{
			PlayerBase attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (attacker)
			{
				TrackingMod_SetPlacer(attacker);
			}
		}
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}
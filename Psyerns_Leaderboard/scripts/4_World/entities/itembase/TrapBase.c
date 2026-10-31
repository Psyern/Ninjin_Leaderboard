modded class TrapBase
{
	private PlayerBase m_TrackingMod_PlayerThatPlaced;
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		PlayerBase placer = PlayerBase.Cast(player);
		if (placer)
		{
			TrackingMod_SetPlayerThatPlaced(placer);
		}
	}
	void TrackingMod_SetPlayerThatPlaced(PlayerBase placer)
	{
		m_TrackingMod_PlayerThatPlaced = placer;
	}
	PlayerBase TrackingMod_GetPlayerThatPlaced()
	{
		return m_TrackingMod_PlayerThatPlaced;
	}
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (!m_TrackingMod_PlayerThatPlaced && source)
		{
			PlayerBase attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (attacker)
			{
				TrackingMod_SetPlayerThatPlaced(attacker);
			}
		}
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}
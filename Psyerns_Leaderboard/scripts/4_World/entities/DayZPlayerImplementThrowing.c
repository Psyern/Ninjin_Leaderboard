modded class DayZPlayerImplementThrowing
{
	override void HandleThrowing(HumanInputController pHic, HumanCommandWeapons pHcw, EntityAI pEntityInHands, float pDt)
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		if (player && pEntityInHands && pEntityInHands.IsInherited(Grenade_Base))
		{
			Grenade_Base grenade = Grenade_Base.Cast(pEntityInHands);
			if (grenade)
			{
				grenade.TrackingMod_SetThrower(player);
			}
		}
		super.HandleThrowing(pHic, pHcw, pEntityInHands, pDt);
	}
}
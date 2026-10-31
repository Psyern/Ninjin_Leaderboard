#ifdef NinjinsPvPPvE
class TrackingModZoneEventCallback extends ZoneEventCallback
{
	override void OnZoneEnter(PlayerBase player, ZoneBase zone)
	{
		if (!player || !player.GetIdentity() || !zone)
			return;
		string playerID = player.GetIdentity().GetPlainId();
		string zoneName = zone.GetName();
		TrackingModZoneUtils.SetPlayerZoneName(playerID, zoneName);
	}
	override void OnZoneExit(PlayerBase player, ZoneBase zone)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerZoneName(playerID, "");
	}
}
class TrackingModZoneHandler extends PlayerStateChangeCallback
{
	override void OnPlayerEnteredPvP(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInPVPZone(playerID, true);
		Print("[TrackingMod] Player entered PVP zone: " + player.GetIdentity().GetName() + " (ID: " + playerID + ")");
	}
	override void OnPlayerExitedPvP(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInPVPZone(playerID, false);
		Print("[TrackingMod] Player exited PVP zone: " + player.GetIdentity().GetName() + " (ID: " + playerID + ")");
	}
	override void OnPlayerEnteredPvE(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInPVEZone(playerID, true);
	}
	override void OnPlayerExitedPvE(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInPVEZone(playerID, false);
	}
	override void OnPlayerEnteredVisual(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInVisualZone(playerID, true);
	}
	override void OnPlayerExitedVisual(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInVisualZone(playerID, false);
	}
	override void OnPlayerEnteredRaid(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInRaidZone(playerID, true);
	}
	override void OnPlayerExitedRaid(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		string playerID = player.GetIdentity().GetPlainId();
		TrackingModZoneUtils.SetPlayerInRaidZone(playerID, false);
	}
}
#endif
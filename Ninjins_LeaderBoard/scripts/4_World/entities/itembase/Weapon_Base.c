modded class Weapon_Base
{
	override void EEFired(int muzzleType, int mode, string ammoType)
	{
		PlayerBase ownerPlayer;
		PlayerIdentity ownerIdentity;
		string ownerID;
		TrackingModData trackData;
		PlayerDeathData ownerData;

		super.EEFired(muzzleType, mode, ammoType);
		if (!g_Game || !g_Game.IsServer())
			return;

		ownerPlayer = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!ownerPlayer)
			return;

		ownerIdentity = ownerPlayer.GetIdentity();
		if (!ownerIdentity)
			return;

		ownerID = ownerIdentity.GetPlainId();
		trackData = TrackingModData.LoadData();
		if (!trackData)
			return;

		ownerData = trackData.GetPlayerData(ownerID);
		if (!ownerData)
			return;

		ownerData.AddShotFired();
		trackData.SavePlayerData(ownerData, ownerID);
	}
}

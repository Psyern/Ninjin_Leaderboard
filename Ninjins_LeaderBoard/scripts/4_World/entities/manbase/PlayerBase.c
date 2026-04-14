modded class PlayerBase
{
	private string m_LastDamageAmmo = "";
	private int m_LastDamageType = -1;
	private EntityAI m_LastDamageSource;
	protected vector m_NJN_LastPosition;
	protected float m_NJN_DistanceAccumulator;
	protected bool m_NJN_PositionInitialized;
	int TrackingMod_GetLastDamageType() { return m_LastDamageType; }
	string TrackingMod_GetLastDamageAmmo() { return m_LastDamageAmmo; }
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		PlayerBase shooterPlayer;
		PlayerIdentity shooterIdentity;
		string shooterID;
		TrackingModData trackData;
		PlayerDeathData shooterData;
		bool isHeadshotHit;

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (g_Game.IsServer())
		{
			m_LastDamageType = damageType;
			m_LastDamageSource = source;
			if (ammo != "")
			{
				m_LastDamageAmmo = ammo;
			}
			if (source)
			{
				shooterPlayer = PlayerBase.Cast(source);
				if (!shooterPlayer && source.IsInherited(EntityAI))
					shooterPlayer = PlayerBase.Cast(EntityAI.Cast(source).GetHierarchyRootPlayer());
				if (shooterPlayer && shooterPlayer != this)
				{
					shooterIdentity = shooterPlayer.GetIdentity();
					if (shooterIdentity)
					{
						shooterID = shooterIdentity.GetPlainId();
						trackData = TrackingModData.LoadData();
						if (trackData)
						{
							shooterData = trackData.GetPlayerData(shooterID);
							if (shooterData)
							{
								isHeadshotHit = (dmgZone == "Head");
								shooterData.AddShotHit(isHeadshotHit);
								trackData.SavePlayerData(shooterData, shooterID);
							}
						}
					}
				}
			}
		}
	}
	override void OnConnect()
	{
		PlayerIdentity identity;
		string plainID;
		string playerName;
		string typeName;
		string survivorType;
		TrackingModData data;
		bool newPlayer;
		PlayerDeathData playerData;
		
		super.OnConnect();
		if (g_Game.IsServer())
		{
			identity = GetIdentity();
			if (identity)
			{
				plainID = identity.GetPlainId();
				playerName = identity.GetName();
				typeName = GetType();
				survivorType = GetSurvivorType();
				Print(string.Format("[TrackingMod] OnConnect - Player: %1 (PlainID: %2), Type: %3, SurvivorType: %4", playerName, plainID, typeName, survivorType));
				
				data = TrackingModData.LoadData();
				newPlayer = false;
				playerData = data.GetOrCreatePlayer(plainID, playerName, newPlayer);
				if (playerData)
				{
					playerData.playerIsOnline = 1;
					playerData.survivorType = survivorType;
					data.UpdateLastLoginDate(playerData);
					data.SavePlayerData(playerData, plainID);
					m_NJN_LastPosition = GetPosition();
					m_NJN_DistanceAccumulator = 0.0;
					m_NJN_PositionInitialized = true;
					Print(string.Format("[TrackingMod] OnConnect - Saved playerData: playerIsOnline=%1, survivorType=%2", playerData.playerIsOnline, playerData.survivorType));
				}
			}
		}
	}
	
	override void OnReconnect()
	{
		PlayerIdentity identity;
		string plainID;
		string playerName;
		string typeName;
		string survivorType;
		TrackingModData data;
		PlayerDeathData playerData;
		
		super.OnReconnect();
		if (g_Game.IsServer())
		{
			identity = GetIdentity();
			if (identity)
			{
				plainID = identity.GetPlainId();
				playerName = identity.GetName();
				typeName = GetType();
				survivorType = GetSurvivorType();
				Print(string.Format("[TrackingMod] OnReconnect - Player: %1 (PlainID: %2), Type: %3, SurvivorType: %4", playerName, plainID, typeName, survivorType));
				
				data = TrackingModData.LoadData();
				playerData = data.GetPlayerData(plainID);
				if (playerData)
				{
					playerData.playerIsOnline = 1;
					playerData.survivorType = survivorType;
					data.UpdateLastLoginDate(playerData);
					data.SavePlayerData(playerData, plainID);
					m_NJN_LastPosition = GetPosition();
					m_NJN_DistanceAccumulator = 0.0;
					m_NJN_PositionInitialized = true;
					Print(string.Format("[TrackingMod] OnReconnect - Saved playerData: playerIsOnline=%1, survivorType=%2", playerData.playerIsOnline, playerData.survivorType));
				}
			}
		}
	}
	
	string GetSurvivorType()
	{
		if (IsMale())
		{
			return "Male";
		}
		else
		{
			return "Female";
		}
	}
	
	void NJN_UpdateDistanceTracking()
	{
		PlayerIdentity distIdentity;
		string distPlainID;
		vector currentPos;
		float dist;
		TrackingModData distTrackData;
		PlayerDeathData distPlayerData;

		if (!g_Game || !g_Game.IsServer())
			return;
		if (!m_NJN_PositionInitialized)
			return;
		if (!IsAlive())
			return;

		currentPos = GetPosition();
		dist = vector.Distance(m_NJN_LastPosition, currentPos);

		if (dist > 0.5 && dist < 1000.0)
		{
			m_NJN_DistanceAccumulator = m_NJN_DistanceAccumulator + dist;
		}
		m_NJN_LastPosition = currentPos;

		if (m_NJN_DistanceAccumulator >= 50.0)
		{
			distIdentity = GetIdentity();
			if (distIdentity)
			{
				distPlainID = distIdentity.GetPlainId();
				distTrackData = TrackingModData.LoadData();
				if (distTrackData)
				{
					distPlayerData = distTrackData.GetPlayerData(distPlainID);
					if (distPlayerData)
					{
						distPlayerData.AddDistanceTravelled(m_NJN_DistanceAccumulator);
						distTrackData.SavePlayerData(distPlayerData, distPlainID);
					}
				}
			}
			m_NJN_DistanceAccumulator = 0.0;
		}
	}

	override void OnDisconnect()
	{
		PlayerIdentity identity;
		string plainID;
		TrackingModData data;
		PlayerDeathData playerData;

		if (g_Game.IsServer())
		{
			identity = GetIdentity();
			if (identity)
			{
				plainID = identity.GetPlainId();
				data = TrackingModData.LoadData();
				playerData = data.GetPlayerData(plainID);
				if (playerData)
				{
					if (m_NJN_PositionInitialized && m_NJN_DistanceAccumulator > 0.5)
					{
						playerData.AddDistanceTravelled(m_NJN_DistanceAccumulator);
						m_NJN_DistanceAccumulator = 0.0;
					}
					m_NJN_PositionInitialized = false;
					playerData.playerIsOnline = 0;
					data.SavePlayerData(playerData, plainID);
				}
			}
		}
		super.OnDisconnect();
	}
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!g_Game.IsServer())
			return;
		TrackDeath(killer);
	}
	void TrackDeath(Object killer)
	{
		PlayerIdentity identity;
		string playerID;
		string playerName;
		string victimID;
		vector deathPosition;
		TrackingModData data;
		string killerName;
		string killerType;
		string killerID;
		int environmentDeathType;
		bool isSelfInflicted;
		bool killedByPlayer;
		bool killedByZombie;
		bool killedByAnimal;
		bool killedByAI;
		PlayerBase killerPlayer;
		PlayerIdentity killerIdentity;
		vector killerPosition;
		float killRange;
		int killCause;
		PlayerBase explosionKiller;
		#ifdef EXPANSIONMODAI
		eAIBase aiKiller;
		Man rootMan;
		#endif
		ZombieBase zombie;
		AnimalBase animal;
		int deathCause;
		
		identity = GetIdentity();
		if (!identity)
			return;
		playerID = identity.GetPlainId();
		playerName = identity.GetName();
		victimID = playerID;
		deathPosition = GetPosition();
		data = TrackingModData.LoadData();
		Print("[TrackingMod] TrackDeath - Player: " + playerName + " (ID: " + playerID + ")");
		killerName = "Unknown";
		killerType = "Unknown";
		killerID = "";
		environmentDeathType = -1;
		killCause = -1;
		if (killer)
		{
			killerType = killer.GetType();
			killerPosition = killer.GetPosition();
			killRange = vector.Distance(deathPosition, killerPosition);
			if (m_LastDamageType == DamageType.EXPLOSION)
			{
				if (m_LastDamageSource)
				{
					explosionKiller = TrackingMod_ResolveExplosiveOwner(m_LastDamageSource);
				}
				if (!explosionKiller && killer && killer.IsInherited(EntityAI))
				{
					explosionKiller = PlayerBase.Cast(EntityAI.Cast(killer).GetHierarchyRootPlayer());
				}
				if (explosionKiller && explosionKiller != this)
				{
					killedByPlayer = true;
					killerIdentity = explosionKiller.GetIdentity();
					if (killerIdentity)
					{
						killerName = killerIdentity.GetName();
						killerID = killerIdentity.GetPlainId();
					Print("[TrackingMod] Killed by player explosion: " + killerName + " from " + killRange.ToString() + " meters");
					killCause = GetKillCause(m_LastDamageType, m_LastDamageAmmo, this);
					data.AddKill(killerID, killerName, killRange, victimID, this, killCause);
					{
						PlayerDeathData explosionCashData;
						int explosionTotalKills;
						explosionCashData = data.GetPlayerData(killerID);
						if (explosionCashData && explosionKiller)
						{
							explosionTotalKills = explosionCashData.GetCategoryKills("Players");
							TrackingModRewardHelper.CheckSimpleCashReward(explosionKiller, killerID, "Players", explosionTotalKills);
						}
					}
					}
					else
					{
						killerName = killer.GetDisplayName();
						Print("[TrackingMod] Killed by explosion: " + killerName);
					}
				}
				else if (explosionKiller && explosionKiller == this)
				{
					isSelfInflicted = true;
					environmentDeathType = 1;
					killerName = playerName;
					killerType = "Self";
					Print("[TrackingMod] Death is self-inflicted explosion");
				}
				else
				{
					environmentDeathType = 1;
					killerName = "Environment";
					killerType = "Environment";
					Print("[TrackingMod] Death is explosion (environment, Type: " + m_LastDamageType.ToString() + ", Ammo: " + m_LastDamageAmmo + ")");
				}
			}
			else
			{
				killerPlayer = PlayerBase.Cast(killer);
				#ifdef EXPANSIONMODAI
				if (!killerPlayer && killer.IsInherited(EntityAI))
				{
					rootMan = EntityAI.Cast(killer).GetHierarchyRootPlayer();
					if (rootMan)
					{
						aiKiller = eAIBase.Cast(rootMan);
						if (!aiKiller)
							killerPlayer = PlayerBase.Cast(rootMan);
					}
				}
				if (!aiKiller)
					aiKiller = eAIBase.Cast(killer);
				if (aiKiller)
				{
					killedByAI = true;
					killerName = aiKiller.GetDisplayName();
					if (killerName == "")
						killerName = killerType;
					Print("[TrackingMod] Killed by AI: " + killerName + " (Type: " + killerType + ") from " + killRange.ToString() + " meters");
				}
				else
				#endif
				{
					if (!killerPlayer && killer.IsInherited(EntityAI))
						killerPlayer = PlayerBase.Cast(EntityAI.Cast(killer).GetHierarchyRootPlayer());
					if (killerPlayer)
					{
						if (killerPlayer == this)
						{
							isSelfInflicted = true;
							killedByPlayer = false;
							if (IsInVehicle() || IsEnvironmentDeath(m_LastDamageType, m_LastDamageAmmo) || IsAgentRelatedDeath())
							{
								environmentDeathType = GetEnvironmentDeathType(m_LastDamageType, m_LastDamageAmmo);
								if (environmentDeathType == -1)
									environmentDeathType = 2;
								if (m_LastDamageAmmo == "FallDamageHealth")
								{
									killerName = playerName;
									killerType = "Self";
								}
								else
								{
									killerName = "Environment";
									killerType = "Environment";
								}
								if (IsInVehicle())
									Print("[TrackingMod] Death is vehicle crash (Type: " + m_LastDamageType.ToString() + ", Ammo: " + m_LastDamageAmmo + ")");
								Print("[TrackingMod] Death is environment (Type: " + m_LastDamageType.ToString() + ", Ammo: " + m_LastDamageAmmo + ", EnvType: " + environmentDeathType.ToString() + ")");
							}
							else
							{
								killerName = playerName;
								killerType = "Self";
								Print("[TrackingMod] Death is self-inflicted");
							}
						}
						else
						{
							killedByPlayer = true;
							killerIdentity = killerPlayer.GetIdentity();
							if (killerIdentity)
							{
								killerName = killerIdentity.GetName();
								killerID = killerIdentity.GetPlainId();
								if (killerPlayer.IsInVehicle())
									Print("[TrackingMod] Killed by player in vehicle: " + killerName + " from " + killRange.ToString() + " meters");
								else
									Print("[TrackingMod] Killed by player: " + killerName + " from " + killRange.ToString() + " meters");
								killCause = GetKillCause(m_LastDamageType, m_LastDamageAmmo, this);
								data.AddKill(killerID, killerName, killRange, victimID, this, killCause);
								{
									PlayerDeathData pvpCashData;
									int pvpTotalKills;
									pvpCashData = data.GetPlayerData(killerID);
									if (pvpCashData && killerPlayer)
									{
										pvpTotalKills = pvpCashData.GetCategoryKills("Players");
										TrackingModRewardHelper.CheckSimpleCashReward(killerPlayer, killerID, "Players", pvpTotalKills);
									}
								}
							}
							else
							{
								killerName = killer.GetDisplayName();
								Print("[TrackingMod] Killed by: " + killerName);
							}
						}
					}
					else
					{
						zombie = ZombieBase.Cast(killer);
						if (zombie)
						{
							killedByZombie = true;
							killerName = killerType;
							Print("[TrackingMod] Killed by zombie: " + killerName);
						}
						else
						{
							animal = AnimalBase.Cast(killer);
							if (animal)
							{
								killedByAnimal = true;
								killerName = killerType;
								Print("[TrackingMod] Killed by animal: " + killerName);
							}
							else
							{
								if (IsFireOrHeatEntity(killer, killerType) || m_LastDamageAmmo == "HeatDamage" || m_LastDamageAmmo == "FireDamage")
								{
									environmentDeathType = 0;
									killerName = "Environment";
									killerType = "Environment";
									Print("[TrackingMod] Death is heat/fire damage from: " + killerType + " (Ammo: " + m_LastDamageAmmo + ")");
								}
								else
								{
									killerName = killer.GetDisplayName();
									Print("[TrackingMod] Killed by entity: " + killerName + " (Type: " + killerType + ")");
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if (IsEnvironmentDeath(m_LastDamageType, m_LastDamageAmmo) || IsAgentRelatedDeath())
			{
				environmentDeathType = GetEnvironmentDeathType(m_LastDamageType, m_LastDamageAmmo);
				if (environmentDeathType == -1)
					environmentDeathType = 2;
				if (m_LastDamageAmmo == "FallDamageHealth")
				{
					isSelfInflicted = true;
					killerName = playerName;
					killerType = "Self";
				}
				else
				{
					killerName = "Environment";
					killerType = "Environment";
				}
				Print("[TrackingMod] Death is environment (Type: " + m_LastDamageType.ToString() + ", Ammo: " + m_LastDamageAmmo + ", EnvType: " + environmentDeathType.ToString() + ")");
			}
			else
			{
				Print("[TrackingMod] Killer unknown");
			}
		}
		deathCause = GetDeathCause(isSelfInflicted, killedByPlayer, killedByZombie, killedByAnimal, killedByAI, environmentDeathType, m_LastDamageType, m_LastDamageAmmo, killer);
		if (g_TrackingModConfig && g_TrackingModConfig.ShouldExcludeDeathCause(deathCause))
		{
			Print("[TrackingMod] Death excluded by config - cause: " + deathCause.ToString() + " (Player: " + playerID + ")");
			return;
		}
		data.UpdatePlayerDeathInfo(playerID, playerName, deathPosition, killerName, killerType, isSelfInflicted, killedByPlayer, killedByZombie, killedByAnimal, killedByAI, environmentDeathType);
		m_LastDamageAmmo = "";
		m_LastDamageType = -1;
	}
	bool IsEnvironmentDeath(int damageType, string ammo)
	{
		if (damageType == DamageType.CUSTOM)
		{
			return true;
		}
		if (damageType == DamageType.EXPLOSION)
		{
			return true;
		}
		if (ammo == "")
			return false;
		if (ammo == "FallDamageHealth" || ammo == "FallDamageShock" || ammo == "FallDamageHealthAttachment" || ammo == "FallDamageHealthOtherAttachments")
		{
			return true;
		}
		if (ammo == "EnviroDmg")
		{
			return true;
		}
		if (ammo == "FireDamage" || ammo == "HeatDamage")
		{
			return true;
		}
		return false;
	}
	int GetEnvironmentDeathType(int damageType, string ammo)
	{
		if (ammo == "HeatDamage" || ammo == "FireDamage")
		{
			return 0;
		}
		if (damageType == DamageType.EXPLOSION)
		{
			return 1;
		}
		if (damageType == DamageType.CUSTOM || ammo == "EnviroDmg")
		{
			return 2;
		}
		return -1;
	}
	bool IsFireOrHeatEntity(Object killer, string killerType)
	{
		if (!killer)
			return false;
		if (killerType == "Fireplace" || killerType == "FireplaceBase" || killerType == "Bonfire" || killerType.IndexOf("Fire") != -1 || killerType.IndexOf("Burning") != -1)
		{
			return true;
		}
		if (killer.IsInherited(FireplaceBase))
		{
			return true;
		}
		if (killer.IsInherited(AreaDamageManager))
		{
			return true;
		}
		return false;
	}
	bool IsAgentRelatedDeath()
	{
		if (!m_AgentPool)
			return false;
		int agents = m_AgentPool.GetAgents();
		if (agents != 0)
		{
			return true;
		}
		float blood = GetHealth("", "Blood");
		bool isBleeding = IsBleeding();
		if (isBleeding && blood < 1000)
		{
			return true;
		}
		return false;
	}
	static PlayerBase TrackingMod_ResolveExplosiveOwner(Object source)
	{
		if (!source)
			return null;
		EntityAI sourceEntity = EntityAI.Cast(source);
		if (!sourceEntity)
			return null;
		PlayerBase thrower;
		PlayerBase placer;
		if (sourceEntity.IsInherited(Grenade_Base))
		{
			Grenade_Base grenade = Grenade_Base.Cast(sourceEntity);
			if (grenade)
			{
				thrower = grenade.TrackingMod_GetThrower();
				if (thrower)
					return thrower;
			}
		}
		if (sourceEntity.IsInherited(ExplosivesBase))
		{
			ExplosivesBase explosive = ExplosivesBase.Cast(sourceEntity);
			if (explosive)
			{
				placer = explosive.TrackingMod_GetPlacer();
				if (placer)
					return placer;
			}
		}
		if (sourceEntity.IsInherited(TrapBase))
		{
			TrapBase trap = TrapBase.Cast(sourceEntity);
			if (trap)
			{
				placer = trap.TrackingMod_GetPlayerThatPlaced();
				if (placer)
					return placer;
			}
		}
		return null;
	}
	int GetDeathCause(bool isSelfInflicted, bool killedByPlayer, bool killedByZombie, bool killedByAnimal, bool killedByAI, int environmentDeathType, int damageType, string ammo, Object killer)
	{
		if (isSelfInflicted)
			return 0;
		if (damageType == DamageType.EXPLOSION || environmentDeathType == 1)
		{
			if (killer && killer.IsInherited(TrapBase))
				return 2;
			return 1;
		}
		if (killer && killer.IsInherited(ZombieBase))
			return 3;
		if (killer && killer.IsInherited(AnimalBase))
			return 4;
		#ifdef EXPANSIONMODAI
		if (killer && killer.IsInherited(eAIBase))
			return 5;
		#endif
		if (killedByPlayer && damageType == DamageType.FIRE_ARM)
			return 7;
		if (killedByPlayer && (damageType == DamageType.CLOSE_COMBAT || ammo == "Melee"))
			return 8;
		if (IsInVehicle() || ammo == "CrashDamage")
			return 6;
		return 9;
	}
	static int GetKillCause(int damageType, string ammo, Object victimEntity)
	{
		if (damageType == DamageType.EXPLOSION)
		{
			if (victimEntity && victimEntity.IsInherited(TrapBase))
				return 1;
			return 0;
		}
		if (ammo == "CrashDamage")
			return 4;
		if (damageType == DamageType.FIRE_ARM)
			return 5;
		if (damageType == DamageType.CLOSE_COMBAT || ammo == "Melee")
			return 6;
		if (victimEntity && victimEntity.IsInherited(AnimalBase))
			return 2;
		if (victimEntity && victimEntity.IsInherited(ZombieBase))
			return 3;
		#ifdef EXPANSIONMODAI
		if (victimEntity && victimEntity.IsInherited(eAIBase))
			return 7;
		#endif
		return 7;
	}
}
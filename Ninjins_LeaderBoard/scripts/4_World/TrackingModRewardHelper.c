class TrackingModRewardHelper
{
	static void ApplyQuantityToItem(ItemBase item, int amount, float quantMin, float quantMax, string itemName)
	{
		Magazine mag;
		float quantityAbsolute;
		int maxAmmo;
		int maxEnergy;
		float energyNormalized;
		int maxQuantity;
		float quantityNormalized;
		
		if (!item || item.GetQuantityMax() <= 0)
			return;
		
		if (amount > 0)
		{
			if (Class.CastTo(mag, item))
			{
				maxAmmo = mag.GetAmmoMax();
				if (amount > maxAmmo)
					amount = maxAmmo;
				mag.ServerSetAmmoCount(amount);
				TrackingMod.LogDebug("[TrackingModRewardHelper] Set magazine ammo (Amount): " + itemName + " x" + amount.ToString());
			}
			else if (item.HasEnergyManager())
			{
				maxEnergy = item.GetQuantityMax();
				energyNormalized = 0.0;
				if (maxEnergy > 0)
				{
					energyNormalized = Math.Clamp((float)amount / (float)maxEnergy, 0.0, 1.0);
				}
				item.GetCompEM().SetEnergy0To1(energyNormalized);
				TrackingMod.LogDebug("[TrackingModRewardHelper] Set energy (Amount): " + itemName + " at " + (energyNormalized * 100.0).ToString() + "%");
			}
			else if (item.HasQuantity())
			{
				maxQuantity = item.GetQuantityMax();
				if (amount > maxQuantity)
					amount = maxQuantity;
				if (amount <= 0 && item.ConfigGetBool("varQuantityDestroyOnMin"))
					amount = 1;
				item.SetQuantity(amount);
				TrackingMod.LogDebug("[TrackingModRewardHelper] Set quantity (Amount): " + itemName + " x" + amount.ToString());
			}
			return;
		}
		
		quantityNormalized = 0.0;
		if (quantMin > 0.0 && quantMax > 0.0 && quantMax >= quantMin)
			quantityNormalized = Math.RandomFloat(quantMin, quantMax);
		else if (quantMin > 0.0)
			quantityNormalized = quantMin;
		else if (quantMax > 0.0)
			quantityNormalized = quantMax;
		if (quantityNormalized <= 0.0)
			return;
		
		if (quantityNormalized > 1.0) quantityNormalized = 1.0;
		if (quantityNormalized < 0.0) quantityNormalized = 0.0;
		
		if (Class.CastTo(mag, item))
		{
			quantityAbsolute = Math.Round(Math.Lerp(0, mag.GetAmmoMax(), quantityNormalized));
			mag.ServerSetAmmoCount(quantityAbsolute);
			TrackingMod.LogDebug("[TrackingModRewardHelper] Set magazine ammo: " + itemName + " x" + quantityAbsolute.ToString() + " (" + (quantityNormalized * 100.0).ToString() + "%)");
		}
		else if (item.HasEnergyManager())
		{
			item.GetCompEM().SetEnergy0To1(quantityNormalized);
			TrackingMod.LogDebug("[TrackingModRewardHelper] Set energy: " + itemName + " at " + (quantityNormalized * 100.0).ToString() + "%");
		}
		else if (item.HasQuantity())
		{
			quantityAbsolute = Math.Round(Math.Lerp(0, item.GetQuantityMax(), quantityNormalized));
			if (quantityAbsolute <= 0 && item.ConfigGetBool("varQuantityDestroyOnMin"))
				quantityAbsolute = 1;
			item.SetQuantity(quantityAbsolute);
			TrackingMod.LogDebug("[TrackingModRewardHelper] Set quantity: " + itemName + " x" + quantityAbsolute.ToString() + " (" + (quantityNormalized * 100.0).ToString() + "%)");
		}
	}
	
	static void ApplyHealthToItem(EntityAI item, float healthMin, float healthMax, string itemName, bool applyToAttachments = false)
	{
		float healthNormalized;
		float maxHealth;
		float healthAbsolute;
		TStringArray dmgZones;
		string dmgZone;
		int i;
		EntityAI attachment;
		int attachIdx;
		float attachMaxHealth;
		TStringArray attachDmgZones;
		string attachDmgZone;
		int j;
		
		if (!item || (healthMin <= 0.0 && healthMax <= 0.0))
			return;
		
		healthNormalized = 0.0;
		if (healthMin > 0.0 && healthMax > 0.0 && healthMax >= healthMin)
			healthNormalized = Math.RandomFloat(healthMin, healthMax);
		else if (healthMin > 0.0)
			healthNormalized = healthMin;
		else if (healthMax > 0.0)
			healthNormalized = healthMax;
		if (healthNormalized <= 0.0)
			return;
		
		if (healthNormalized > 1.0) healthNormalized = 1.0;
		if (healthNormalized < 0.0) healthNormalized = 0.0;
		
		maxHealth = MiscGameplayFunctions.GetTypeMaxGlobalHealth(item.GetType());
		if (maxHealth <= 0.0)
			return;
		
		healthAbsolute = maxHealth * healthNormalized;
		item.SetHealth("", "", healthAbsolute);
		
		dmgZones = {};
		item.GetDamageZones(dmgZones);
		for (i = 0; i < dmgZones.Count(); i++)
		{
			dmgZone = dmgZones.Get(i);
			item.SetHealth01(dmgZone, "Health", healthNormalized);
		}
		
		if (applyToAttachments && item.GetInventory())
		{
			for (attachIdx = 0; attachIdx < item.GetInventory().AttachmentCount(); attachIdx++)
			{
				attachment = item.GetInventory().GetAttachmentFromIndex(attachIdx);
				if (attachment)
				{
					attachMaxHealth = MiscGameplayFunctions.GetTypeMaxGlobalHealth(attachment.GetType());
					if (attachMaxHealth > 0.0)
					{
						attachment.SetHealth("", "", attachMaxHealth * healthNormalized);
						attachDmgZones = {};
						attachment.GetDamageZones(attachDmgZones);
						for (j = 0; j < attachDmgZones.Count(); j++)
						{
							attachDmgZone = attachDmgZones.Get(j);
							attachment.SetHealth01(attachDmgZone, "Health", healthNormalized);
						}
					}
				}
			}
		}
		TrackingMod.LogDebug("[TrackingModRewardHelper] Set health: " + itemName + " at " + (healthNormalized * 100.0).ToString() + "% (" + healthAbsolute.ToString() + "/" + maxHealth.ToString() + ")");
	}
	
	static void ProcessAttachmentsRecursive(EntityAI parentEntity, array<ref TrackingModRewardAttachment> attachmentConfigs, string parentItemName, int depth)
	{
		if (!parentEntity || !attachmentConfigs || attachmentConfigs.Count() == 0)
			return;
		
		int j;
		ItemBase attachmentItemBase;
		EntityAI attachment;
		TrackingModRewardAttachment attachmentConfig;
		float randomChance;
		
		for (j = 0; j < attachmentConfigs.Count(); j++)
		{
			attachmentConfig = attachmentConfigs.Get(j);
			if (!attachmentConfig || attachmentConfig.ItemClassName == "")
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] Invalid attachment config at index " + j.ToString() + " for " + parentItemName);
				continue;
			}
			
			randomChance = Math.RandomFloat(0.0, 100.0);
			if (randomChance > attachmentConfig.SpawnChance)
			{
				TrackingMod.LogDebug("[TrackingModRewardHelper] Attachment " + attachmentConfig.ItemClassName + " failed spawn chance (" + randomChance.ToString() + " > " + attachmentConfig.SpawnChance.ToString() + "). Skipping.");
				continue;
			}
			
			attachment = parentEntity.GetInventory().CreateInInventory(attachmentConfig.ItemClassName);
			if (!attachment)
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] Failed to create attachment: " + attachmentConfig.ItemClassName + " for " + parentItemName);
				continue;
			}
			
			attachmentItemBase = ItemBase.Cast(attachment);
			if (attachmentItemBase)
			{
				ApplyQuantityToItem(attachmentItemBase, attachmentConfig.Amount, attachmentConfig.QuantMin, attachmentConfig.QuantMax, attachmentConfig.ItemClassName);
			}
			ApplyHealthToItem(attachment, attachmentConfig.HealthMin, attachmentConfig.HealthMax, attachmentConfig.ItemClassName, false);
			attachment.SetSynchDirty();
			TrackingMod.LogDebug("[TrackingModRewardHelper] Attached " + attachmentConfig.ItemClassName + " to " + parentItemName + " (depth: " + depth.ToString() + ")");
			
			if (attachmentConfig.Attachments && attachmentConfig.Attachments.Count() > 0)
			{
				TrackingMod.LogDebug("[TrackingModRewardHelper] Processing " + attachmentConfig.Attachments.Count().ToString() + " nested attachment configs for " + attachmentConfig.ItemClassName);
				ProcessAttachmentsRecursive(attachment, attachmentConfig.Attachments, attachmentConfig.ItemClassName, depth + 1);
			}
		}
	}
	
	static bool AddItemsToPlayerInventory(PlayerBase player, array<ref TrackingModRewardItem> rewardItems)
	{
		if (!IsMissionHost())
			return false;
		if (!player || !player.GetIdentity())
			return false;
		if (!rewardItems || rewardItems.Count() == 0)
		{
			TrackingMod.LogCritical("[TrackingModRewardHelper] No reward items to add to inventory!");
			return false;
		}
		
		int i;
		TrackingModRewardItem rewardItem;
		EntityAI tempItem;
		InventoryLocation tempLocation;
		bool canFit;
		EntityAI rewardEntity;
		ItemBase rewardItemBase;
		string playerName;
		bool validationFailed;
		int itemsAddedCount;
		int itemsOnGroundCount;
		vector playerPos;
		vector playerDir;
		vector groundPos;
		string resultMessage;
		
		playerName = player.GetIdentity().GetName();
		TrackingMod.LogInfo("[TrackingModRewardHelper] Checking if all " + rewardItems.Count().ToString() + " reward items can fit in player inventory: " + playerName);
		
		validationFailed = false;
		for (i = 0; i < rewardItems.Count(); i++)
		{
			rewardItem = rewardItems.Get(i);
			if (!rewardItem || rewardItem.ItemClassName == "")
			{
				TrackingMod.LogCritical("[TrackingModRewardHelper] Invalid reward item at index " + i.ToString() + ". Cannot add items to inventory.");
				validationFailed = true;
				break;
			}
			
			tempItem = EntityAI.Cast(g_Game.CreateObject(rewardItem.ItemClassName, vector.Zero, false, true));
			if (!tempItem)
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] Item class not found: " + rewardItem.ItemClassName + ". Cannot add items to inventory.");
				validationFailed = true;
				break;
			}
			
			tempLocation = new InventoryLocation;
			canFit = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.CARGO, tempLocation);
			if (!canFit)
			{
				canFit = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.ANY, tempLocation);
			}
			
			g_Game.ObjectDelete(tempItem);
			if (!canFit)
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] No space in player inventory for item: " + rewardItem.ItemClassName + ". All items must fit - cannot add partial reward.");
				validationFailed = true;
				break;
			}
		}
		
		if (validationFailed)
		{
			TrackingMod.LogInfo("[TrackingModRewardHelper] Some items cannot fit in inventory - will place on ground if needed: " + playerName);
		}
		
		TrackingMod.LogInfo("[TrackingModRewardHelper] Creating " + rewardItems.Count().ToString() + " items for player: " + playerName);
		
		itemsAddedCount = 0;
		itemsOnGroundCount = 0;
		playerPos = player.GetPosition();
		playerDir = player.GetDirection();
		
		for (i = 0; i < rewardItems.Count(); i++)
		{
			rewardItem = rewardItems.Get(i);
			
			tempItem = EntityAI.Cast(g_Game.CreateObject(rewardItem.ItemClassName, vector.Zero, false, true));
			if (!tempItem)
			{
				TrackingMod.LogCritical("[TrackingModRewardHelper] Item class not found during creation: " + rewardItem.ItemClassName + ". Skipping item.");
				continue;
			}
			tempLocation = new InventoryLocation;
			canFit = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.CARGO, tempLocation);
			if (!canFit)
			{
				canFit = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.ANY, tempLocation);
			}
			g_Game.ObjectDelete(tempItem);
			
			if (canFit)
			{
				rewardEntity = player.GetHumanInventory().CreateInInventory(rewardItem.ItemClassName);
				if (!rewardEntity)
				{
					canFit = false;
				}
			}
			
			if (!canFit || !rewardEntity)
			{
				groundPos = playerPos + (playerDir * 1.5);
				groundPos[1] = playerPos[1];
				rewardEntity = player.SpawnEntityOnGroundPos(rewardItem.ItemClassName, groundPos);
				if (!rewardEntity)
				{
					TrackingMod.LogWarning("[TrackingModRewardHelper] Failed to create item on ground: " + rewardItem.ItemClassName + ". Skipping item.");
					continue;
				}
				itemsOnGroundCount++;
				TrackingMod.LogInfo("[TrackingModRewardHelper] Placed item on ground (inventory full): " + rewardItem.ItemClassName);
			}
			else
			{
				itemsAddedCount++;
				TrackingMod.LogInfo("[TrackingModRewardHelper] Added item to player inventory: " + rewardItem.ItemClassName);
			}
			
			rewardItemBase = ItemBase.Cast(rewardEntity);
			if (rewardItemBase)
			{
				ApplyQuantityToItem(rewardItemBase, rewardItem.Amount, rewardItem.QuantMin, rewardItem.QuantMax, rewardItem.ItemClassName);
			}
			ApplyHealthToItem(rewardEntity, rewardItem.HealthMin, rewardItem.HealthMax, rewardItem.ItemClassName, false);
			rewardEntity.SetSynchDirty();
			
			if (rewardItem.Attachments && rewardItem.Attachments.Count() > 0)
			{
				TrackingMod.LogDebug("[TrackingModRewardHelper] Processing " + rewardItem.Attachments.Count().ToString() + " attachment configs for " + rewardItem.ItemClassName);
				ProcessAttachmentsRecursive(rewardEntity, rewardItem.Attachments, rewardItem.ItemClassName, 1);
			}
		}
		
		if (itemsAddedCount + itemsOnGroundCount == rewardItems.Count())
		{
			if (itemsOnGroundCount > 0)
			{
				resultMessage = "Successfully added " + itemsAddedCount.ToString() + " item(s) to inventory and placed " + itemsOnGroundCount.ToString() + " item(s) on ground: " + playerName;
				TrackingMod.LogInfo("[TrackingModRewardHelper] " + resultMessage);
				if (player && player.GetIdentity())
				{
					NotificationSystem.Create(new StringLocaliser("Reward Claimed"), new StringLocaliser("Some items were placed on the ground due to full inventory."), "Ninjins_Tracking_Mod/gui/success.edds", ARGB(255, 255, 165, 0), 5.0, player.GetIdentity());
				}
			}
			else
			{
				resultMessage = "Successfully added all " + itemsAddedCount.ToString() + " item(s) to player inventory: " + playerName;
				TrackingMod.LogInfo("[TrackingModRewardHelper] " + resultMessage);
			}
			return true;
		}
		else
		{
			TrackingMod.LogCritical("[TrackingModRewardHelper] Failed to spawn all items - expected " + rewardItems.Count().ToString() + " but only spawned " + (itemsAddedCount + itemsOnGroundCount).ToString() + "!");
			if (player && player.GetIdentity())
			{
				NotificationSystem.Create(new StringLocaliser("Reward Claim Failed"), new StringLocaliser("Failed to spawn all reward items. Please try again."), "Ninjins_Tracking_Mod/gui/error.edds", ARGB(255, 255, 0, 0), 5.0, player.GetIdentity());
			}
			return false;
		}
	}
	
	static void SetCurrencyQuantity(ItemBase item, int amount, string className)
	{
		int quantityToSet;
		int maxQuantity;
		
		if (!item)
			return;
		if (item.GetQuantityMax() > 0)
		{
			quantityToSet = amount;
			maxQuantity = item.GetQuantityMax();
			if (quantityToSet > maxQuantity)
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] Currency amount " + quantityToSet.ToString() + " exceeds max quantity " + maxQuantity.ToString() + " for " + className + ". Clamping to max.");
				quantityToSet = maxQuantity;
			}
			item.SetQuantity(quantityToSet);
			TrackingMod.LogDebug("[TrackingModRewardHelper] Set currency quantity: " + className + " x" + quantityToSet.ToString());
		}
		else
		{
			TrackingMod.LogWarning("[TrackingModRewardHelper] Currency item " + className + " does not support quantity (GetQuantityMax() = 0). Spawning with default quantity.");
		}
	}
	
	static void AddCurrencyToPlayerInventory(PlayerBase player, array<ref TrackingModCurrencyReward> currencyRewards)
	{
		if (!IsMissionHost())
			return;
		if (!player)
		{
			TrackingMod.LogCritical("[TrackingModRewardHelper] Player is null for currency rewards!");
			return;
		}
		if (!currencyRewards || currencyRewards.Count() == 0)
		{
			TrackingMod.LogInfo("[TrackingModRewardHelper] No currency rewards to add to player inventory.");
			return;
		}
		
		TrackingMod.LogInfo("[TrackingModRewardHelper] Adding " + currencyRewards.Count().ToString() + " currency rewards to player inventory: " + player.GetIdentity().GetName());
		
		int i;
		TrackingModCurrencyReward currencyReward;
		EntityAI currencyEntity;
		ItemBase itemBase;
		InventoryLocation tempLocation;
		EntityAI tempItem;
		bool canFitInCargo;
		vector playerPos;
		vector playerDir;
		vector groundPos;
		
		for (i = 0; i < currencyRewards.Count(); i++)
		{
			currencyReward = currencyRewards.Get(i);
			if (!currencyReward || currencyReward.ClassName == "" || currencyReward.Amount <= 0)
			{
				TrackingMod.LogCritical("[TrackingModRewardHelper] Invalid currency reward at index " + i.ToString());
				continue;
			}
			
			tempItem = EntityAI.Cast(g_Game.CreateObject(currencyReward.ClassName, vector.Zero, false, true));
			if (!tempItem)
			{
				TrackingMod.LogWarning("[TrackingModRewardHelper] Currency class not found: " + currencyReward.ClassName + ". Skipping currency.");
				continue;
			}
			
			tempLocation = new InventoryLocation;
			canFitInCargo = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.CARGO, tempLocation);
			if (!canFitInCargo)
			{
				canFitInCargo = player.GetHumanInventory().FindFreeLocationFor(tempItem, FindInventoryLocationType.ANY, tempLocation);
			}
			
			g_Game.ObjectDelete(tempItem);
			
			if (canFitInCargo)
			{
				currencyEntity = player.GetHumanInventory().CreateInInventory(currencyReward.ClassName);
			}
			
			if (!currencyEntity)
			{
				playerPos = player.GetPosition();
				playerDir = player.GetDirection();
				groundPos = playerPos + (playerDir * 1.5);
				groundPos[1] = playerPos[1];
				currencyEntity = player.SpawnEntityOnGroundPos(currencyReward.ClassName, groundPos);
				if (!currencyEntity)
				{
					TrackingMod.LogWarning("[TrackingModRewardHelper] Failed to create currency item on ground: " + currencyReward.ClassName + ". Skipping currency.");
					continue;
				}
				TrackingMod.LogInfo("[TrackingModRewardHelper] Placed currency on ground (inventory full): " + currencyReward.ClassName);
			}
			
			itemBase = ItemBase.Cast(currencyEntity);
			if (itemBase)
			{
				SetCurrencyQuantity(itemBase, currencyReward.Amount, currencyReward.ClassName);
			}
			currencyEntity.SetSynchDirty();
		}
		
		TrackingMod.LogInfo("[TrackingModRewardHelper] Successfully processed all currency rewards for player: " + player.GetIdentity().GetName());
	}
	
	static TrackingModRewardItem SelectRandomRewardItem(array<ref TrackingModRewardItem> rewardItems)
	{
		float totalChance;
		int i;
		TrackingModRewardItem item;
		float randomValue;
		float accumulatedChance;
		
		if (!rewardItems || rewardItems.Count() == 0)
			return null;
		
		totalChance = 0.0;
		for (i = 0; i < rewardItems.Count(); i++)
		{
			item = rewardItems.Get(i);
			if (item && item.ItemClassName != "")
			{
				totalChance = totalChance + item.SpawnChance;
			}
		}
		
		if (totalChance <= 0.0)
		{
			for (i = 0; i < rewardItems.Count(); i++)
			{
				item = rewardItems.Get(i);
				if (item && item.ItemClassName != "")
				{
					return item;
				}
			}
			return null;
		}
		
		randomValue = Math.RandomFloat(0.0, totalChance);
		accumulatedChance = 0.0;
		
		for (i = 0; i < rewardItems.Count(); i++)
		{
			item = rewardItems.Get(i);
			if (item && item.ItemClassName != "")
			{
				accumulatedChance = accumulatedChance + item.SpawnChance;
				if (randomValue <= accumulatedChance)
				{
					return item;
				}
			}
		}
		
		for (i = rewardItems.Count() - 1; i >= 0; i--)
		{
			item = rewardItems.Get(i);
			if (item && item.ItemClassName != "")
			{
				return item;
			}
		}
		
		return null;
	}
	
	static TrackingModCurrencyReward SelectRandomCurrencyReward(array<ref TrackingModCurrencyReward> currencyRewards)
	{
		float totalChance;
		int i;
		TrackingModCurrencyReward currency;
		float randomValue;
		float accumulatedChance;
		
		if (!currencyRewards || currencyRewards.Count() == 0)
			return null;
		
		totalChance = 0.0;
		for (i = 0; i < currencyRewards.Count(); i++)
		{
			currency = currencyRewards.Get(i);
			if (currency && currency.ClassName != "")
			{
				totalChance = totalChance + currency.SpawnChance;
			}
		}
		
		if (totalChance <= 0.0)
		{
			for (i = 0; i < currencyRewards.Count(); i++)
			{
				currency = currencyRewards.Get(i);
				if (currency && currency.ClassName != "")
				{
					return currency;
				}
			}
			return null;
		}
		
		randomValue = Math.RandomFloat(0.0, totalChance);
		accumulatedChance = 0.0;
		
		for (i = 0; i < currencyRewards.Count(); i++)
		{
			currency = currencyRewards.Get(i);
			if (currency && currency.ClassName != "")
			{
				accumulatedChance = accumulatedChance + currency.SpawnChance;
				if (randomValue <= accumulatedChance)
				{
					return currency;
				}
			}
		}
		
		for (i = currencyRewards.Count() - 1; i >= 0; i--)
		{
			currency = currencyRewards.Get(i);
			if (currency && currency.ClassName != "")
			{
				return currency;
			}
		}
		
		return null;
	}
	
	static array<ref TrackingModCurrencyReward> SelectCurrencyRewards(array<ref TrackingModCurrencyReward> currencyRewards, int currencyMin, int currencyMax)
	{
		array<ref TrackingModCurrencyReward> currenciesToGive;
		int currenciesToPick;
		int currencyRetryCount;
		int maxCurrencyRetries;
		array<ref TrackingModCurrencyReward> availableCurrencies;
		int i;
		TrackingModCurrencyReward currency;
		int j;
		int pickedCount;
		TrackingModCurrencyReward selectedCurrency;
		array<ref TrackingModCurrencyReward> tempAvailableCurrencies;
		
		currenciesToGive = new array<ref TrackingModCurrencyReward>;
		if (!currencyRewards || currencyRewards.Count() == 0)
			return currenciesToGive;
		
		if (currencyMax > currencyMin)
			currenciesToPick = Math.RandomInt(currencyMin, currencyMax + 1);
		else
			currenciesToPick = currencyMin;
		
		if (currenciesToPick > currencyRewards.Count())
			currenciesToPick = currencyRewards.Count();
		
		if (currenciesToPick <= 0)
			return currenciesToGive;
		
		currencyRetryCount = 0;
		maxCurrencyRetries = 20;
		
		while (currenciesToGive.Count() == 0 && currencyRetryCount < maxCurrencyRetries)
		{
			currencyRetryCount++;
			currenciesToGive.Clear();
			
			availableCurrencies = new array<ref TrackingModCurrencyReward>;
			for (i = 0; i < currencyRewards.Count(); i++)
			{
				currency = currencyRewards.Get(i);
				if (currency && currency.ClassName != "")
				{
					availableCurrencies.Insert(currency);
				}
			}
			
			pickedCount = 0;
			tempAvailableCurrencies = new array<ref TrackingModCurrencyReward>;
			for (i = 0; i < availableCurrencies.Count(); i++)
			{
				tempAvailableCurrencies.Insert(availableCurrencies.Get(i));
			}
			
			while (pickedCount < currenciesToPick && tempAvailableCurrencies.Count() > 0)
			{
				selectedCurrency = SelectRandomCurrencyReward(tempAvailableCurrencies);
				if (selectedCurrency)
				{
					currenciesToGive.Insert(selectedCurrency);
					for (j = tempAvailableCurrencies.Count() - 1; j >= 0; j--)
					{
						if (tempAvailableCurrencies.Get(j) == selectedCurrency)
						{
							tempAvailableCurrencies.Remove(j);
							break;
						}
					}
					pickedCount = pickedCount + 1;
				}
				else
				{
					break;
				}
			}
		}
		
		if (currencyRetryCount >= maxCurrencyRetries && currenciesToGive.Count() == 0)
		{
			TrackingMod.LogCritical("[TrackingModRewardHelper] Failed to pick currencies after " + maxCurrencyRetries.ToString() + " retries!");
		}
		
		return currenciesToGive;
	}
}

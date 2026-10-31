class TrackingModMilestoneResult
{
	string CategoryID;
	int Milestone;
	
	void TrackingModMilestoneResult()
	{
		CategoryID = "";
		Milestone = 0;
	}
}

class TrackingModMilestoneHelper
{
	static int CalculatePendingRewards(PlayerDeathData playerData)
	{
		int totalPending;
		string categoryID;
		array<int> availableMilestones;
		int categoryIdx;
		
		if (!playerData || !g_TrackingModRewardConfig)
			return 0;
		
		if (!g_TrackingModRewardConfig.EnableMilestoneRewards)
			return 0;
		
		totalPending = 0;
		
		// Count all milestones in AvailableMilestones
		if (playerData.AvailableMilestones)
		{
			for (categoryIdx = 0; categoryIdx < playerData.AvailableMilestones.Count(); categoryIdx++)
			{
				categoryID = playerData.AvailableMilestones.GetKey(categoryIdx);
				availableMilestones = playerData.AvailableMilestones.GetElement(categoryIdx);
				if (availableMilestones)
				{
					totalPending = totalPending + availableMilestones.Count();
				}
			}
		}
		
		return totalPending;
	}
	
	static TrackingModMilestoneResult GetNextAvailableMilestone(PlayerDeathData playerData)
	{
		TrackingModMilestoneResult result;
		string categoryID;
		array<int> availableMilestones;
		int categoryIdx;
		int milestoneIdx;
		int milestone;
		
		result = new TrackingModMilestoneResult();
		
		if (!playerData || !g_TrackingModRewardConfig || !g_TrackingModRewardConfig.EnableMilestoneRewards)
			return result;
		
		// Get first available milestone from AvailableMilestones
		if (playerData.AvailableMilestones && playerData.AvailableMilestones.Count() > 0)
		{
			for (categoryIdx = 0; categoryIdx < playerData.AvailableMilestones.Count(); categoryIdx++)
			{
				categoryID = playerData.AvailableMilestones.GetKey(categoryIdx);
				availableMilestones = playerData.AvailableMilestones.GetElement(categoryIdx);
				if (availableMilestones && availableMilestones.Count() > 0)
				{
					// Get first milestone from this category
					milestone = availableMilestones.Get(0);
					result.CategoryID = categoryID;
					result.Milestone = milestone;
					return result;
				}
			}
		}
		
		return result;
	}
	
	static void MarkMilestoneAsClaimed(PlayerDeathData playerData, string categoryID, int milestone)
	{
		array<int> claimedMilestones;
		array<int> availableMilestones;
		int availableIdx;
		bool found;
		
		if (!playerData)
			return;
		
		// Remove from AvailableMilestones
		if (playerData.AvailableMilestones && playerData.AvailableMilestones.Contains(categoryID))
		{
			availableMilestones = playerData.AvailableMilestones.Get(categoryID);
			if (availableMilestones)
			{
				found = false;
				for (availableIdx = availableMilestones.Count() - 1; availableIdx >= 0; availableIdx--)
				{
					if (availableMilestones.Get(availableIdx) == milestone)
					{
						availableMilestones.Remove(availableIdx);
						found = true;
						TrackingMod.LogInfo("[TrackingModMilestoneHelper] Removed milestone " + milestone.ToString() + " from AvailableMilestones for category " + categoryID);
						break;
					}
				}
				
				// If no more milestones for this category, remove the category entry
				if (availableMilestones.Count() == 0)
				{
					playerData.AvailableMilestones.Remove(categoryID);
				}
			}
		}
		
		// Add to ClaimedMilestones
		if (!playerData.ClaimedMilestones)
			playerData.ClaimedMilestones = new map<string, ref array<int>>();
		
		if (!playerData.ClaimedMilestones.Contains(categoryID))
		{
			claimedMilestones = new array<int>();
			playerData.ClaimedMilestones.Set(categoryID, claimedMilestones);
		}
		else
		{
			claimedMilestones = playerData.ClaimedMilestones.Get(categoryID);
		}
		
		if (claimedMilestones)
		{
			claimedMilestones.Insert(milestone);
			TrackingMod.LogInfo("[TrackingModMilestoneHelper] Added milestone " + milestone.ToString() + " to ClaimedMilestones for category " + categoryID);
		}
	}
}

class TrackingModPvPLeaderboardController: ViewController
{
	string PlayerOnlineCounter = "Online: 0";
	string TopX = "Top 10";
	ref ObservableCollection<ref TrackingModPvPPlayerEntry> PlayerEntries = new ObservableCollection<ref TrackingModPvPPlayerEntry>(this);
}

class TrackingModLeaderboardController: ViewController
{
	string PlayerOnlineCounter = "Online: 0";
	string TopX = "Top 10";
	ref ObservableCollection<ref TrackingModPlayerEntry> PlayerEntries = new ObservableCollection<ref TrackingModPlayerEntry>(this);
}

class TrackingModPlayerEntryController: ViewController
{
	string PlayerName = "";
	string PlayerPosition = "0";
	string PVEPoints = "0";
	ref ObservableCollection<ref TrackingModKillCategory> KillCategories = new ObservableCollection<ref TrackingModKillCategory>(this);
}
